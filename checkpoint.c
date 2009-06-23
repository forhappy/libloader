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

static int logger_fd = 0;

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
	return __write(logger_fd, &regs, sizeof(regs));
}

SCOPE int
after_syscall(struct syscall_regs regs)
{
	return __write(logger_fd, &regs, sizeof(regs));
}

// vim:ts=4:sw=4

