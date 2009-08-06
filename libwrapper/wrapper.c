/* 
 * wrapper.c
 * by WN @ Jun. 22, 2009
 */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "injector.h"
#include "checkpoint/checkpoint.h"
#include "injector_utils.h"
#include "injector_debug.h"
#include "pthread_defs.h"

extern void __vsyscall();

static char help_string[] = "This is help string\n";
pid_t SCOPE self_pid = 0;
/* for replay use */
pid_t SCOPE old_self_pid = 0;

/* we define this var because this struct need to be aligned by 16
 * bytes. if we use local var, we need special align work */
struct i387_fxsave_struct SCOPE fpustate_struct __attribute__((aligned(16)));

static int replay = 0;

static int logger_threshold = 10 << 20;	/* 10MB */


SCOPE void
show_help(void)
{
	INTERNAL_SYSCALL_int80(write, 3, 1, help_string, sizeof(help_string));
	memset(&state_vector, '\0', sizeof(state_vector));
	memset(buffer, '\0', BUFFER_SIZE);
	printf_int80("state_vector at %p\n", &state_vector);
	printf_int80("buffer at %p\n", buffer);
	INTERNAL_SYSCALL_int80(exit, 1, 0);
}


SCOPE char logger_filename[128] = "";
SCOPE char ckpt_filename[128] = "";

SCOPE volatile enum syscall_status syscall_status = OUT_OF_SYSCALL;

SCOPE const struct syscall_regs * current_regs;

SCOPE void
wrapped_syscall(const struct syscall_regs r)
{
	if (!replay) {
		/* we save current regs file for signal checkpoint use. */
		current_regs = &r;

		uint32_t syscall_nr = r.orig_eax;
		INJ_SILENT("wrapped_syscall: %d\n", syscall_nr);
		before_syscall(&r);
		syscall_status = IN_SYSCALL;
		/* before we call real vsyscall, we must restore register
		 * state */
		uint32_t retval;
		asm volatile(
				"pushl %%eax\n"
				"pushl %%ebx\n"
				"pushl %%ecx\n"
				"pushl %%edx\n"
				"pushl %%esi\n"
				"pushl %%edi\n"
				"pushl %%ebp\n"
				"movl %P1, %%eax\n"
				"movl %P2, %%ebx\n"
				"movl %P3, %%ecx\n"
				"movl %P4, %%edx\n"
				"movl %P5, %%esi\n"
				"movl %P6, %%edi\n"
				"movl %P7, %%ebp\n"
				"call __vsyscall\n"
				"popl %%ebp\n"
				"popl %%edi\n"
				"popl %%esi\n"
				"popl %%edx\n"
				"popl %%ecx\n"
				"popl %%ebx\n"
				"addl $0x4, %%esp\n"	/* eax */
				"movl %%eax, %P1\n"
				: "=a" (retval)
				: "m" (r.eax), "m" (r.ebx), "m" (r.ecx), 
				"m" (r.edx), "m" (r.esi), "m" (r.edi),
				"m" (r.ebp));

		/* syscall_status == OUT_OF_SYSCALL, this means we come back here by
		 * load a ckpt. this shouldn't happen, think about SA_RESTORER. */
		ASSERT(syscall_status != OUT_OF_SYSCALL, "ckeck point come to the wrong place\n");

		if (syscall_status >= SIGNALED) {
			/* last syscall is disturbed by a signal, the logger has been switched */
			/* we redo before_syscall. most of syscall has no real pre_handler. */
			if (syscall_table[syscall_nr].pre_handler != NULL) {
				INJ_WARNING("syscall %d interrupted by signal %d, logger may inconsistent\n",
						syscall_nr, syscall_status - SIGNALED);
			}
			int32_t real_eax = retval;
			((struct syscall_regs*)(&r))->eax = syscall_nr;
			before_syscall(&r);
			((struct syscall_regs*)(&r))->eax = real_eax;

			/* we still need to write this flag */
			int16_t f = -1;
			INTERNAL_SYSCALL(write, 3, logger_fd, &f, sizeof(f));
		} else if (syscall_status == IN_SYSCALL){
			/* write a flag to indicate syscall not be distrubed */
			/* if this syscall distrubed, the next data in logger
			 * may be a syscall_nr (if signal processing use syscall) or a -2
			 * (write by wrapped_(rt_)sigreturn). when replay, if see this -1,
			 * we know this syscall ends normally. */
			int16_t f = -1;
			INTERNAL_SYSCALL(write, 3, logger_fd, &f, sizeof(f));
		} else {
			INJ_FATAL("!@#!#$^%#%@#$\n");
			INTERNAL_SYSCALL(exit, 1, -1);
		}

		after_syscall(&r);
		syscall_status = OUT_OF_SYSCALL;

		/* in the assembly code, we have modify the 'eax'. */

		/* check the logger sz */
		INJ_SILENT("logger_sz = %d\n", logger_sz);
		if (logger_sz > logger_threshold) {
			int err;

			/* we need to remake ckpt */
			INJ_TRACE("make ckpt: eip=0x%x\n", r.eip);
			/* we still need to adjust esp:
			 * when we come here, r.esp hold an 'ret' address for
			 * coming 'ret'. */
			((struct syscall_regs*)(&r))->esp += 4;
			/* save fpustate */
			save_i387(&fpustate_struct);
			make_checkpoint(ckpt_filename, (struct syscall_regs *)(&r),
					&fpustate_struct, NULL);
			((struct syscall_regs*)(&r))->esp -= 4;

			/* truncate logger file */
			err = INTERNAL_SYSCALL(ftruncate, 2, logger_fd, 0);
			ASSERT(err == 0, "ftruncate failed: %d\n", err);

			/* reset logger_sz */
			/* logger_sz have been reset in do_make_checkpoint */
			/* logger_sz = 0; */
		}

	} else {
		INJ_TRACE("syscall, eax=%d\n", r.eax);
		uint32_t retval;
		retval = replay_syscall(&r);
		/* here we force to reset the eax */
		(((volatile struct syscall_regs *)&r)->eax) = retval;
	}
}

extern void wrapped_sigreturn(void);
extern void wrapped_rt_sigreturn(void);

SCOPE void
injector_entry(struct syscall_regs r,
		uint32_t old_vdso_ventry, int threshold,
		uint32_t main_addr)
{
	int err;

	/* build STDOUT_FILENO_INJ and STDERR_FILENO_INJ */
	/* all INJ_XXXX messages are wrote to console through
	 * STDOUT_FILENO_INJ and STDERR_FILENO_INJ, not the original
	 * 1 and 2. This is for some targets which close 1 and 2, like
	 * lighttpd.
	 * STDOUT_FILENO_INJ and STDERR_FILENO_INJ are defined in injector_debug.h */
	if (STDOUT_FILENO != STDOUT_FILENO_INJ)
		INTERNAL_SYSCALL(dup2, 2, STDOUT_FILENO, STDOUT_FILENO_INJ);
	if (STDERR_FILENO != STDERR_FILENO_INJ)
		INTERNAL_SYSCALL(dup2, 2, STDERR_FILENO, STDERR_FILENO_INJ);


	INJ_TRACE("Here! we come to injector!!!\n");
	INJ_TRACE("wrapped_sigreturn=%p\n", wrapped_sigreturn);
	INJ_TRACE("wrapped_rt_sigreturn=%p\n", wrapped_rt_sigreturn);
	INJ_TRACE("%d, 0x%x, 0x%x\n", threshold, old_vdso_ventry, main_addr);

	ASSERT(threshold >= 4096, "logger threshold %d is strange\n", threshold);

	logger_threshold = threshold;

	old_self_pid = self_pid = INTERNAL_SYSCALL(getpid, 0);

	snprintf(logger_filename, 128, LOGGER_DIRECTORY"/%d.log", self_pid);
	INJ_TRACE("logger fn: %s\n", logger_filename);

	snprintf(ckpt_filename, 128, LOGGER_DIRECTORY"/%d.ckpt", self_pid);
	INJ_TRACE("ckpt fn: %s\n", ckpt_filename);

	int fd = INTERNAL_SYSCALL(open, 3, logger_filename, O_WRONLY|O_APPEND, 0666);
	INJ_TRACE("logger fd = %d\n", fd);
	ASSERT(fd > 0, "open logger failed: %d\n", fd);

	/* dup the fd to LOGGER_FD */
	err = INTERNAL_SYSCALL(dup2, 2, fd, LOGGER_FD);
	ASSERT(err == LOGGER_FD, "dup2 failed: %d\n", err);
	INJ_TRACE("dup fd to %d\n", err);

	err = INTERNAL_SYSCALL(close, 1, fd);
	ASSERT(err == 0, "close failed: %d\n", err);
	INJ_TRACE("close %d\n", fd);

	logger_fd = LOGGER_FD;

	/* :NOTICE: */
	/* We MUST adjust esp here. when loader call injector, it pushs
	 * 3 int32_t onto stack, so the esp in 'r' cannot be used directly.
	 */
	save_i387(&fpustate_struct);
	r.esp += 12;
	make_checkpoint(ckpt_filename, &r, &fpustate_struct, NULL);
	r.esp -= 12;

	err = INTERNAL_SYSCALL(ftruncate, 2, logger_fd, 0);
	ASSERT(err == 0, "ftruncate failed: %d\n", err);
	INJ_TRACE("main ip=0x%x:0x%x\n", main_addr, r.eip);
	INJ_TRACE("eax=%d\n", r.eax);
}

static void
restore_state(void)
{
	/* brk */
	/* brk should have been reset in gdbloader. */
	uint32_t addr;
	addr = INTERNAL_SYSCALL(brk, 1, state_vector.brk);
	ASSERT(addr == state_vector.brk, "brk failed");

	/* clear_child_tid */
	/* always success */
	INTERNAL_SYSCALL(set_tid_address, 1, state_vector.clear_child_tid);

	/* robust_list */
	INTERNAL_SYSCALL(set_robust_list, 2, state_vector.robust_list,
			sizeof(struct robust_list_head));

	/* thread_areas */
	for (int i = 0; i < GDT_ENTRY_TLS_ENTRIES; i++) {
		if (state_vector.thread_area[i].entry_number != -1) {
			/* this slot is setted */
			INTERNAL_SYSCALL(set_thread_area, 1,
					&state_vector.thread_area[i]);
		}
	}

	/* sigaction */
	/* don't restore sigaction now */

	/* sigmask */
	/* don't restore sigmask now */

#define SIG_BLOCK          1	/* for blocking signals */
#define SIG_UNBLOCK        2	/* for unblocking signals */
#define SIG_SETMASK        3	/* for setting the signal mask */
	INTERNAL_SYSCALL(rt_sigprocmask, 4,
			SIG_SETMASK, &state_vector.sigmask, NULL, sizeof(state_vector.sigmask));
}

static void
fix_libpthread(pt_list_t * sym_stack_user,
		pt_list_t * sym_stack_used)
{
	if ((sym_stack_user == NULL) || (sym_stack_used == NULL))
		return;

	int nr_thread = 0;

	pt_list_t * pos;
	pt_list_for_each(pos, sym_stack_user) {
		nr_thread ++;
		int * tid, * pid;
		tid = (int*)((void*)(pos) + 8);
		pid = (int*)((void*)(pos) + 12);
		INJ_TRACE("find thread %d: tid=%x, pid=%d\n", nr_thread, * tid, * pid);
		if (nr_thread == 1) {
			if (*tid != *pid) {
				INJ_WARNING("1st thread tid=%d, pid=%d, doesn't support\n",
						*tid, *pid);
			} else {
				if (*tid != old_self_pid) {
					INJ_WARNING("1st thread tid=%d, pid=%d, target pid=%d, doesn't support\n",
							*tid, *pid, old_self_pid);
				} else {
					INJ_WARNING("fixing pthread's tid and pid: \n");
					INJ_WARNING("reset old tid and pid from %d to %d\n", *tid, self_pid);
					INJ_WARNING("this may cause unfaithful replay\n", *tid, self_pid);
					*tid = self_pid;
					*pid = self_pid;
				}
			}
		}
	}

	pt_list_for_each(pos, sym_stack_used) {
		nr_thread ++;
		int * tid, * pid;
		tid = (int*)((void*)(pos) + 8);
		pid = (int*)((void*)(pos) + 12);
		INJ_TRACE("find thread %d: tid=%x, pid=%d\n", nr_thread, * tid, * pid);
	}

	if (nr_thread != 1) {
		INJ_WARNING("we have %d threads, it doesn't support now.\n",
				nr_thread);
	}
}


#if 0
static void
restore_libpthread(pt_list_t * sym_stack_user,
		pt_list_t * sym_stack_used)
{
	if ((sym_stack_user == NULL) || (sym_stack_used == NULL))
		return;

	int nr_thread = 0;
	pt_list_t * pos;
	pt_list_for_each(pos, sym_stack_user) {
		nr_thread ++;
		int * tid, * pid;
		tid = (int*)((void*)(pos) + 8);
		pid = (int*)((void*)(pos) + 12);
		INJ_TRACE("find thread %d: tid=%x, pid=%d\n", nr_thread, * tid, * pid);
		if (nr_thread == 1) {
			if (*tid != *pid) {
				INJ_WARNING("1st thread tid=%d, pid=%d, doesn't support\n",
						*tid, *pid);
			} else {
				if (*tid != self_pid) {
					INJ_WARNING("1st thread tid=%d, pid=%d, target pid=%d, doesn't support\n",
							*tid, *pid, old_self_pid);
				} else {
					*tid = old_self_pid;
					*pid = old_self_pid;
				}
			}
		}
	}
}
#endif

SCOPE void
debug_entry(struct syscall_regs r,
		void * sym_stack_user,
		void * sym_stack_used,
		void * entry)
{
	/* see injector_entry */
	if (STDOUT_FILENO != STDOUT_FILENO_INJ)
		INTERNAL_SYSCALL(dup2, 2, STDOUT_FILENO, STDOUT_FILENO_INJ);
	if (STDERR_FILENO != STDERR_FILENO_INJ)
		INTERNAL_SYSCALL(dup2, 2, STDERR_FILENO, STDERR_FILENO_INJ);

	/* only reset self_pid, old_self_pid still be the original pid. */
	self_pid = INTERNAL_SYSCALL(getpid, 0);

	/* from state_vector, restore state */
	restore_state();

	fix_libpthread(sym_stack_user, sym_stack_used);

	/* set replay to 1 */
	replay = 1;

	/* open logger */
	int fd = INTERNAL_SYSCALL(open, 3, logger_filename,
			O_RDONLY, 0666);
	INJ_TRACE("logger fd = %d\n", fd);
	ASSERT(fd > 0, "open logger failed: %d\n", fd);

	/* dup the fd to LOGGER_FD */
	int err = INTERNAL_SYSCALL(dup2, 2, fd, LOGGER_FD);
	ASSERT(err == LOGGER_FD, "dup2 failed: %d\n", err);
	INJ_TRACE("logger fd dupped to %d\n", err);

	err = INTERNAL_SYSCALL(close, 1, fd);
	ASSERT(err == 0, "close fd %d failed: %d\n", fd, err);
	INJ_TRACE("close %d\n", fd);

	logger_fd = LOGGER_FD;

	/* most of registers should have been restored in loader.
	 * gs is special, poke gs only restore thread.gs in kernel's code,
	 * we have to restore the real gs manually. */
	asm volatile(
		"movw %%ax, %%gs\n"
		:
		: "a" (state_vector.regs.gs));

	/* restore fpustate */
	/* fpustate_struct is aligned */
	memcpy(&fpustate_struct, &state_vector.fpustate, sizeof(fpustate_struct));
	restore_i387(&fpustate_struct);

	/* restore syscall_status to OUT_OF_SYSCALL */
	syscall_status = OUT_OF_SYSCALL;

	/* spin */
	volatile int xxx = 0;
	INJ_FORCE("state has been restored, run gdb:\n");
	INJ_FORCE("\t(gdb) attach %d\n", self_pid);
	INJ_FORCE("\t(gdb) p *(int*)(0x%x) = 1\n", &xxx);
	INJ_FORCE("\t(gdb) b *0x%x\n", entry);
	INJ_FORCE("\t(gdb) c\n");
	while (xxx == 0) {
		struct timespec {
			long       ts_sec;
			long       ts_nsec;
		};
		struct timespec tm = {1, 0};
		INTERNAL_SYSCALL(nanosleep, 2, &tm, NULL);
	}

	/* if restore tid and pid, gdb will crash */
/* 	restore_libpthread(sym_stack_user, sym_stack_used); */
}

// vim:ts=4:sw=4

