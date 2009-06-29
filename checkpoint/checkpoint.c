/* 
 * recorder.c
 * by WN @ Jun. 22, 2009
 */

#include "checkpoint.h"
#include <unistd.h>
#include <fcntl.h>


#ifndef INJECTOR
#include <stdio.h>
#include <stdlib.h>
#endif

SCOPE struct state_vector state_vector = {
	.dummy	= 0,
	.brk	= 0,
	.clear_child_tid	= 0,
	.robust_list	= 0,
};

int SCOPE logger_fd = 0;

SCOPE int
checkpoint_init(void)
{
	return 0;
}

SCOPE int
logger_init(pid_t pid)
{
#ifndef INJECTOR
	int fd;
	char filename[64];
	snprintf(filename, 64, "/var/lib/currf2/%d.log",
			pid);
	fd = __open(filename, O_WRONLY|O_APPEND|O_CREAT, 0666);
	if (fd < 0) {
		__printf("open log file %s failed: %d\n", filename, fd);
		return fd;
	}

	logger_fd = fd;
	return 0;
#endif
}


SCOPE int
logger_close(void)
{
	__close(logger_fd);
	return 0;
}

SCOPE int
checkpoint(void)
{
	return 0;
}

SCOPE int
before_syscall(struct syscall_regs regs)
{
	__printf("before syscall at eip=0x%x\n", regs.eip);
	if (regs.orig_eax >= NR_SYSCALLS) {
		__printf("no such syscall: %d\n", regs.orig_eax);
		__exit(0);
	}

	if (syscall_table[regs.orig_eax].pre_handler != NULL)
		return syscall_table[regs.orig_eax].pre_handler(&regs);
	return 0;
}

SCOPE int
after_syscall(struct syscall_regs regs)
{
	if (regs.orig_eax >= NR_SYSCALLS) {
		__printf("no such syscall: %d\n", regs.orig_eax);
		__exit(0);
	}

	if (syscall_table[regs.orig_eax].post_handler != NULL) {
		return syscall_table[regs.orig_eax].post_handler(&regs);
	} else {
		__printf("no such syscall post-handler: %d\n", regs.orig_eax);
		__exit(0);
	}
	return 0;
}

// vim:ts=4:sw=4

