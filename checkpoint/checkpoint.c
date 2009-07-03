/* 
 * recorder.c
 * by WN @ Jun. 22, 2009
 */

#include "checkpoint.h"
#include <unistd.h>
#include <fcntl.h>


#ifndef IN_INJECTOR
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

SCOPE char *
readline(int fd)
{
#define READLINE_BUFFER_SIZE	(2048)
#define HALF_READLINE_BUFFER_SIZE	(READLINE_BUFFER_SIZE/2)
	static char readline_buffer[READLINE_BUFFER_SIZE];
#define half_readline_buffer (&readline_buffer[HALF_READLINE_BUFFER_SIZE])
	static char * p_nxtl = readline_buffer;
	static char * p_nxtb = readline_buffer;

	if (p_nxtb == readline_buffer) {
		int sz;
		memset(readline_buffer, '\0', READLINE_BUFFER_SIZE);
		sz = __read(fd, readline_buffer, READLINE_BUFFER_SIZE);
		p_nxtb = readline_buffer + sz;
	}

	char * p;
restart:
	p = p_nxtl;
	while (p < p_nxtb) {
		if ((*p == '\n') || (*p == '\0')) {
			char * p_nxtl_save = p_nxtl;
			if (*p == '\n')
				p_nxtl = p+1;
			else
				p_nxtl = p;
			*p = '\0';
			return p_nxtl_save;
		}
		p++;
	}

	/* file is short enough that only half of
	 * the readline buffer is OK. */
	if (p < readline_buffer + READLINE_BUFFER_SIZE)
		return p;

	int sz;
	if (p_nxtl < half_readline_buffer) {
		INJ_WARNING("line too long: %s\n", p_nxtl);
		p_nxtl = half_readline_buffer;
	}


	/* move the bottom half to the upper half */
	memcpy(readline_buffer, half_readline_buffer,
			HALF_READLINE_BUFFER_SIZE);
	memset(half_readline_buffer, '\0', HALF_READLINE_BUFFER_SIZE);
	p_nxtl -= HALF_READLINE_BUFFER_SIZE;
	sz = __read(fd, half_readline_buffer, HALF_READLINE_BUFFER_SIZE);
	p_nxtb = half_readline_buffer + sz;
	goto restart;
}


SCOPE int
checkpoint_init(void)
{
	return 0;
}

SCOPE int
logger_init(pid_t pid)
{
#ifndef IN_INJECTOR
	int fd;
	char filename[64];
	memset(&state_vector, '\0', sizeof(state_vector));
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
	return 0;
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
before_syscall(struct syscall_regs * regs)
{
	if (regs->orig_eax >= NR_SYSCALLS) {
		__printf("no such syscall: %d\n", regs->orig_eax);
		__exit(0);
	}

	if (syscall_table[regs->orig_eax].pre_handler != NULL)
		return syscall_table[regs->orig_eax].pre_handler(regs);
	return 0;
}

SCOPE int
after_syscall(struct syscall_regs * regs)
{
	if (regs->orig_eax >= NR_SYSCALLS) {
		__printf("no such syscall: %d\n", regs->orig_eax);
		__exit(0);
	}

	if (syscall_table[regs->orig_eax].post_handler != NULL) {
		return syscall_table[regs->orig_eax].post_handler(regs);
	} else {
		__printf("no such syscall post-handler: %d\n", regs->orig_eax);
		__exit(0);
	}
	return 0;
}

static void ATTR(unused)
do_make_checkpoint(int ckpt_fd, int maps_fd)
{
	char * p = readline(maps_fd);
	while (p != NULL)
		INJ_WARNING("%s\n", p);
}

SCOPE void
make_checkpoint(const char * ckpt_fn)
{
#ifdef IN_INJECTOR
	int maps_fd, ckpt_fd;
	maps_fd = INTERNAL_SYSCALL(open, 2,
			"/proc/self/maps", O_RDONLY);
	ASSERT(maps_fd > 0, "open self maps failed: %d\n", maps_fd);
	ckpt_fd = INTERNAL_SYSCALL(open, 3,
			ckpt_fn, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	ASSERT(ckpt_fd > 0, "open ckpt file failed: %d\n", ckpt_fd);

	do_make_checkpoint(ckpt_fd, maps_fd);

	INTERNAL_SYSCALL(close, 1, ckpt_fd);
	INTERNAL_SYSCALL(close, 1, maps_fd);

	return;
#else
	return;
#endif
}

// vim:ts=4:sw=4

