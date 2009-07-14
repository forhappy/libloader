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


static char help_string[] = "This is help string\n";
pid_t SCOPE self_pid = 0;

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


static uint32_t current_syscall = 0;
SCOPE char logger_filename[64] = "";
SCOPE char ckpt_filename[64] = "";

SCOPE void
__before_syscall(struct syscall_regs r)
{
	current_syscall = r.eax;
	INJ_TRACE("before syscall %d\n", current_syscall);
	INJ_TRACE("eip = 0x%x\n", r.eip);
	before_syscall(&r);
	return;
}

SCOPE void
__after_syscall(struct syscall_regs r)
{
	r.orig_eax = current_syscall;
	INJ_TRACE("after syscall %d\n", current_syscall);
	after_syscall(&r);
	return;
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

// vim:ts=4:sw=4

