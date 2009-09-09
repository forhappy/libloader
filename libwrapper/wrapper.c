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
#include "signal_defs.h"

extern void __vsyscall();

static char help_string[] = "This is help string\n";
pid_t SCOPE self_pid = 0;
/* for replay use */
pid_t SCOPE old_self_pid = 0;

/* we define this var because this struct need to be aligned by 16
 * bytes. if we use local var, we need special align work */
struct i387_fxsave_struct SCOPE fpustate_struct __attribute__((aligned(16)));

int SCOPE replay = 0;
int SCOPE tracing = 0;

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


/* this counter is used for signal processing. when sigreturn called, the
 * processing check the counter, if it is 0, then we know the signal doesn't
 * break a syscall. if it is not 0, then signal disturbs a syscall. we use
 * asm here for atomic. */
/* We merge the 2 counter into a structure */
SCOPE volatile struct syscall_latch __syscall_latch = {.u.v = 0};

#ifndef RELAX_SIGNAL
static k_sigset_t blockall_mask = {
	.sig = {0xffffffffUL, 0xffffffffUL},
};
# define DISABLE_SIGNAL() do { \
	INTERNAL_SYSCALL(rt_sigprocmask, 4, SIG_SETMASK, &blockall_mask, NULL, sizeof(k_sigset_t));\
} while(0)
# define ENABLE_SIGNAL() do { \
	INTERNAL_SYSCALL(rt_sigprocmask, 4, SIG_SETMASK, &state_vector.sigmask, NULL, sizeof(k_sigset_t));\
} while(0)
#else
# define DISABLE_SIGNAL() do{ }while(0)
# define ENABLE_SIGNAL() do{ }while(0)
#endif


SCOPE const struct syscall_regs * signal_regs;

static inline int
is_fork_syscall(int nr)
{
	if (nr == __NR_fork)
		return 1;
	if (nr == __NR_vfork)
		return 1;
	if (nr == __NR_clone)
		return 1;
	return 0;
}

/* before we call real vsyscall, we must restore register
 * state */
#define call_syscall(__r, __retval) \
		asm volatile(	\
				"pushl %%eax\n"	\
				"pushl %%ebx\n"	\
				"pushl %%ecx\n"	\
				"pushl %%edx\n"	\
				"pushl %%esi\n"	\
				"pushl %%edi\n"	\
				"pushl %%ebp\n"	\
				"movl %P1, %%eax\n"	\
				"movl %P2, %%ebx\n"	\
				"movl %P3, %%ecx\n"	\
				"movl %P4, %%edx\n"	\
				"movl %P5, %%esi\n"	\
				"movl %P6, %%edi\n"	\
				"movl %P7, %%ebp\n"	\
				"call __vsyscall\n"	\
				"popl %%ebp\n"	\
				"popl %%edi\n"	\
				"popl %%esi\n"	\
				"popl %%edx\n"	\
				"popl %%ecx\n"	\
				"popl %%ebx\n"	\
				"addl $0x4, %%esp\n"	/* eax */	\
				"movl %%eax, %P1\n"	\
				: "=a" ((__retval))	\
				: "m" ((__r).eax), "m" ((__r).ebx), "m" ((__r).ecx), 	\
				"m" ((__r).edx), "m" ((__r).esi), "m" ((__r).edi),	\
				"m" ((__r).ebp))


SCOPE void
wrapped_syscall(const struct syscall_regs r)
{
	/* if we don't trace fork and clone, the
	 * child process can set tracing to 0 */
	if (!tracing) {
		uint32_t retval;
		call_syscall(r, retval);
		return;
	}

	/* we must set signal_regs very early. if we set it
	 * inside ENTER/EXIT_SYSCALL, a signal may happen before this assignment. */
	/* don't check IS_BREAK_SYSCALL. we allow embeded signal handler. */
	/* if a signal raise, we will use signal_regs immediately. so don't worry about
	 * override. (the signal handler will save this regs before any new syscall, and
	 * the restorer needn't it). */
	signal_regs = &r;
	
	/* if a signal raise outside a syscall, those 2 values should same. if not, thay are
	 * different. */
	__syscall_reenter_base = __syscall_reenter_counter;

	/* if a signal happened after this gate and before EXIT_SYSCALL,
	 * we know this syscall is disturbed, when sigprocess making ckpts,
	 * it needs to adjust registers. */


	/* don't allow signal inside before_syscall.
	 * if signal happened inside it, the logger may be disturbed. */
	/* disable signal before enter_syscall make sure the sysall's header
	 * is written into log when potential signal raise */
	DISABLE_SIGNAL();
	ENTER_SYSCALL();

	if (!replay) {
		uint32_t syscall_nr = r.orig_eax;
		INJ_SILENT("wrapped_syscall: %d\n", syscall_nr);

		before_syscall(&r);
		ENABLE_SIGNAL();

		uint32_t retval;

		call_syscall(r, retval);

		DISABLE_SIGNAL();

		/* write a flag to indicate syscall not be disturbed */
		/* if this syscall disturbed, the next data in logger
		 * may be a syscall_nr (if signal processing use syscall) or a -2
		 * (write by wrapped_(rt_)sigreturn). when replay, if see this -1,
		 * we know this syscall ends normally. */
		int16_t f = -1;
		if (!(is_fork_syscall(syscall_nr) && (retval == 0))) {
			int err;
			err = INTERNAL_SYSCALL(write, 3, logger_fd, &f, sizeof(f));
			ASSERT(err == sizeof(f), &r, "write signal tag failed: %d\n", err);
			logger_sz += err;
		}
		/* don't allow signal in after_syscall. if signal happens
		 * in it, the syscall handler may only write a part of log into
		 * log file. when ckpt is made and resume, it will write the left,
		 * makes the log file inconsistent. */
		after_syscall(&r);

		INJ_SILENT("wrapped_syscall: %d over, retval=%d\n", syscall_nr, r.eax);

		/* in the assembly code, we have modify the 'eax'. */

		/* check the logger sz */
		INJ_SILENT("logger_sz = %d\n", logger_sz);
		/* don't switch when signal processing */
		if ((logger_sz > injector_opts.logger_threshold) &&
				(!IS_REENTER_SYSCALL())) {

			/* we need to remake ckpt */
			INJ_TRACE("make ckpt: eip=0x%x\n", r.eip);
			/* we still need to adjust esp:
			 * when we come here, r.esp hold an 'ret' address for
			 * coming 'ret'. */
			((struct syscall_regs*)(&r))->esp += 4;
			/* save fpustate */
			save_i387(&fpustate_struct);
			/* don't keep the old ckpt and log */
			make_checkpoint((struct syscall_regs *)(&r),
					&fpustate_struct, NULL, FALSE);
			((struct syscall_regs*)(&r))->esp -= 4;
		}
		EXIT_SYSCALL();
		ENABLE_SIGNAL();
	} else {
		/* this gate is useless in replay */
		EXIT_SYSCALL();
		ENABLE_SIGNAL();
		INJ_SILENT("replay syscall, eax=%d\n", r.eax);
		uint32_t retval;
		retval = replay_syscall(&r);
		INJ_SILENT("syscall, eax=%d, replayed: %d\n", r.eax, retval);
		/* here we force to reset the eax */
		(((volatile struct syscall_regs *)&r)->eax) = retval;
	}
}

extern void wrapped_sigreturn(void);
extern void wrapped_rt_sigreturn(void);

SCOPE void
injector_entry(struct syscall_regs r,
		uint32_t old_vdso_ventry,
		uint32_t main_addr)
{
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

	ASSERT(injector_opts.logger_threshold >= 4096, &r, "logger threshold %d is strange\n",
			injector_opts.logger_threshold);

	old_self_pid = self_pid = INTERNAL_SYSCALL(getpid, 0);

	if (injector_opts.untraced) {
		/* We need to restore sighandler's restorer here */
		tracing = 0;
		unhook_sighandlers();
		return;
	}

	tracing = 1;

	/* initiate logger and ckpt filename. there's no ckpt now. */
	/* the logger_filename should be reset while making ckpt. */
	snprintf(logger_filename, 128, LOGGER_DIRECTORY"/%d.log", self_pid);
	INJ_TRACE("logger fn: %s\n", logger_filename);
	/* the ckpt_filename should be reset in make_checkpoint,
	 * set it to "" to notice make_checkpoint that there's no old ckpt now */
	ckpt_filename[0] = '\0';

	/* we haven't open it */
	logger_fd = -1;

	/* save the sigprocmask */
	INTERNAL_SYSCALL(rt_sigprocmask, 4,
			0, NULL, &state_vector.sigmask, sizeof(state_vector.sigmask));

	/* :NOTICE: */
	/* We MUST adjust esp here. when loader call injector, it pushs
	 * 2 int32_t onto stack, so the esp in 'r' cannot be used directly.
	 */
	save_i387(&fpustate_struct);
	r.esp += 8;
	/* don't keep the old ckpt */
	make_checkpoint(&r, &fpustate_struct, NULL, FALSE);
	r.esp -= 8;

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
	ASSERT(addr == state_vector.brk, NULL, "brk failed");

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
			O_RDONLY, 0664);
	INJ_TRACE("logger fd = %d\n", fd);
	ASSERT(fd > 0, &r, "open logger failed: %d\n", fd);

	/* dup the fd to LOGGER_FD */
	int err = INTERNAL_SYSCALL(dup2, 2, fd, LOGGER_FD);
	ASSERT(err == LOGGER_FD, &r, "dup2 failed: %d\n", err);
	INJ_TRACE("logger fd dupped to %d\n", err);

	err = INTERNAL_SYSCALL(close, 1, fd);
	ASSERT(err == 0, &r, "close fd %d failed: %d\n", fd, err);
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

	/* restore reenter counter */
	ASSERT(__syscall_reenter_counter <= 1, &r,
			"__syscall_reenter_counter=%d\n", __syscall_reenter_counter);
	__syscall_reenter_counter = 0;

	/* spin */
	if (!injector_opts.nowait) {
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
	}
	/* if restore tid and pid, gdb will crash */
/* 	restore_libpthread(sym_stack_user, sym_stack_used); */
}

// vim:ts=4:sw=4

