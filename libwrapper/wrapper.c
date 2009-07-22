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

extern void __vsyscall();

static char help_string[] = "This is help string\n";
pid_t SCOPE self_pid = 0;

static int replay = 0;

SCOPE void
show_help(void)
{
	INTERNAL_SYSCALL_int80(write, 3, 1, help_string, sizeof(help_string));
	memset(&state_vector, '\0', sizeof(state_vector));
	memset(buffer, '\0', BUFFER_SIZE);
	printf_int80("state_vector at %p\n", &state_vector);
	printf_int80("buffer at %p\n", buffer);
	while(1);
}


SCOPE char logger_filename[64] = "";
SCOPE char ckpt_filename[64] = "";

SCOPE void
__before_syscall(const struct syscall_regs * r)
{
	INJ_TRACE("eip = 0x%x\n", r->eip);
	before_syscall(r);
	return;
}

SCOPE void
__after_syscall(const struct syscall_regs * r)
{
	after_syscall(r);
	return;
}

SCOPE void
wrapped_syscall(const struct syscall_regs r)
{
	if (!replay) {
		__before_syscall(&r);
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
		__after_syscall(&r);
		/* in the assembly code, we have modify the 'eax'. */
	} else {
		while(1);
	}
}


SCOPE void
injector_entry(struct syscall_regs r,
		uint32_t old_vdso_ventry, uint32_t old_vdso_vhdr, uint32_t main_addr)
{
	int err;

	INJ_TRACE("Here! we come to injector!!!\n");
	INJ_TRACE("0x%x, 0x%x, 0x%x\n", old_vdso_vhdr, old_vdso_ventry, main_addr);

	self_pid = INTERNAL_SYSCALL(getpid, 0);

	snprintf(logger_filename, 64, LOGGER_DIRECTORY"/%d.log", self_pid);
	INJ_TRACE("logger fn: %s\n", logger_filename);

	snprintf(ckpt_filename, 64, LOGGER_DIRECTORY"/%d.ckpt", self_pid);
	INJ_TRACE("ckpt fn: %s\n", ckpt_filename);

	int fd = INTERNAL_SYSCALL(open, 3, logger_filename, O_WRONLY|O_APPEND, 0666);
	INJ_TRACE("logger fd = %d\n", fd);
	ASSERT(fd > 0, "open logger failed: %d\n", fd);

	/* dup the fd to 1023 */
	err = INTERNAL_SYSCALL(dup2, 2, fd, 1023);
	ASSERT(err == 1023, "dup2 failed: %d\n", err);
	INJ_TRACE("dup fd to 1023\n");

	err = INTERNAL_SYSCALL(close, 1, fd);
	ASSERT(err == 0, "close failed: %d\n", err);
	INJ_TRACE("close %d\n", fd);

	logger_fd = 1023;

	make_checkpoint(ckpt_filename, &r);
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
}

SCOPE void
debug_entry(void)
{
	/* this function never return */
	INJ_FORCE("come into target code...\n");
	/* from state_vector, restore state */
	restore_state();
	/* set replay to 1 */
	replay = 1;
	while(1);
}

// vim:ts=4:sw=4

