/* 
 * recorder.c
 * by WN @ Jun. 22, 2009
 */

#include "checkpoint.h"
#include <sys/mman.h>
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
			if (*p_nxtl_save == '\0')
				return NULL;
			return p_nxtl_save;
		}
		p++;
	}

	/* file is short enough that only half of
	 * the readline buffer is OK. */
	if (p < readline_buffer + READLINE_BUFFER_SIZE) {
		if (*p == '\0')
			return NULL;
		return p;
	}

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

	/* write syscall nr */
	write_syscall_nr(regs->orig_eax);
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

#ifdef IN_INJECTOR

struct mem_region {
	uint32_t start;
	uint32_t end;
	uint32_t prot;
	uint32_t offset;
	uint32_t fn_len;
	char fn[0];
};

static void ATTR(unused)
do_make_checkpoint(int ckpt_fd, int maps_fd, struct syscall_regs * r)
{
	char * p = readline(maps_fd);
	while (p != NULL) {
		void * start, * end;
		int l = -1;
		uint32_t offset;
		char prot[5]="XXXX\0";

		INJ_TRACE("%s\n", p);

		sscanf(p, "%lx-%lx %4s %x %*2d:%*2d %*d %n",
				&start, &end, prot, &offset, &l);

		INJ_TRACE("start=%p, end=%p, prot=%s, offset=0x%x l=%d\n",
				start, end, prot, offset, l);
		if (l > 0)
			INJ_TRACE("fn: %s\n", p + l);

		struct mem_region m;
		m.start = (uint32_t)start;
		m.end = (uint32_t)end;
		m.prot = 0;
		if (prot[0] == 'r')
			m.prot |= PROT_READ;
		if (prot[1] == 'w')
			m.prot |= PROT_WRITE;
		if (prot[2] == 'x')
			m.prot |= PROT_EXEC;
		m.offset = offset;
		m.fn_len = 0;
		if (l > 0)
			m.fn_len = strlen(p + l) + 1;

		__write(ckpt_fd, &m, sizeof(m));
		if (l > 0)
		__write(ckpt_fd, p + l, m.fn_len);

		__write(ckpt_fd, start, end-start);

		p = readline(maps_fd);
	}

	/* write state vector */
	__write(ckpt_fd, &state_vector, sizeof(state_vector));
}
#endif

SCOPE void
make_checkpoint(const char * ckpt_fn, struct syscall_regs * r)
{
#ifdef IN_INJECTOR
	int maps_fd, ckpt_fd;
	maps_fd = INTERNAL_SYSCALL(open, 2,
			"/proc/self/maps", O_RDONLY);
	ASSERT(maps_fd > 0, "open self maps failed: %d\n", maps_fd);
	ckpt_fd = INTERNAL_SYSCALL(open, 3,
			ckpt_fn, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	ASSERT(ckpt_fd > 0, "open ckpt file failed: %d\n", ckpt_fd);

	do_make_checkpoint(ckpt_fd, maps_fd, r);

	INTERNAL_SYSCALL(close, 1, ckpt_fd);
	INTERNAL_SYSCALL(close, 1, maps_fd);

//	__exit(-1);
	return;
#else
	return;
#endif
}

// vim:ts=4:sw=4

