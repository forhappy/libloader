/* 
 * recorder.c
 * by WN @ Jun. 22, 2009
 */

#include "checkpoint.h"
#include <linux/user.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>


#ifndef IN_INJECTOR
# include <stdio.h>
# include <stdlib.h>
#else
# include "injector_debug.h"
#endif

SCOPE struct state_vector state_vector = {
	.dummy	= 0,
	.brk	= 0,
	.clear_child_tid	= 0,
	.robust_list	= 0,
};

SCOPE struct injector_opts injector_opts = {
	.logger_threshold	= 10 << 20,
	.trace_fork			= 0,
};

int SCOPE logger_fd = 0;
uint32_t SCOPE logger_sz = 0;

SCOPE char *
readline(int fd)
{
#define READLINE_BUFFER_SIZE	(2048)
#define HALF_READLINE_BUFFER_SIZE	(READLINE_BUFFER_SIZE/2)
	static char readline_buffer[READLINE_BUFFER_SIZE];
#define half_readline_buffer (&readline_buffer[HALF_READLINE_BUFFER_SIZE])
	static char * p_nxtl = readline_buffer;
	static char * p_nxtb = readline_buffer;

	if (fd == -1) {
		/* fd == -1 means reset readline vars */
		p_nxtl = readline_buffer;
		p_nxtb = readline_buffer;
		return NULL;
	}

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

	/* for each thread_area, set its entry_number to -1, to indicate
	 * which slot is not used */
	for (int i = 0; i < GDT_ENTRY_TLS_ENTRIES; i++)
		state_vector.thread_area[i].entry_number = -1;

	snprintf(filename, 64, LOGGER_DIRECTORY"/%d.log",
			pid);
	fd = __open(filename, O_WRONLY|O_APPEND|O_CREAT, 0664);
	if (fd < 0) {
		INJ_WARNING("open log file %s failed: %d\n", filename, fd);
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
before_syscall(const struct syscall_regs * regs)
{
	ASSERT(regs->orig_eax < NR_SYSCALLS, regs,
			"before: no such syscall: %d\n", regs->orig_eax);

	/* write syscall nr */
	write_syscall_nr(regs->orig_eax);
	if (syscall_table[regs->orig_eax].pre_handler != NULL)
		return syscall_table[regs->orig_eax].pre_handler(regs);
	return 0;
}

SCOPE int
after_syscall(const struct syscall_regs * regs)
{
	ASSERT(regs->orig_eax < NR_SYSCALLS, regs,
			"no such syscall: %d\n", regs->orig_eax);
	ASSERT(syscall_table[regs->orig_eax].post_handler != NULL, regs,
			"no such syscall post-handler: %d\n", regs->orig_eax);

	return syscall_table[regs->orig_eax].post_handler(regs);
}

#ifdef IN_INJECTOR

extern void SCOPE ATTR(noreturn)
replay_sighandler(int signum, const struct syscall_regs * regs);

SCOPE uint32_t
replay_syscall(const struct syscall_regs * regs)
{

	ASSERT(regs->orig_eax < NR_SYSCALLS, regs,
			"no such syscall: %d\n", regs->orig_eax);

	/* read from logger, check */
	int16_t nr = read_int16();
	if (nr != regs->orig_eax) {
		if ((nr < 0) && (nr > - K_NSIG - 1)) {
			INJ_FORCE("target has been interrupted by signal %d, you need switch ckpt\n", -nr-1);
			replay_trap(regs);
		}
		INJ_FATAL("logger mismatch: new syscall should be 0x%x, but actually %d\n",
				nr, regs->orig_eax);
		/* int3 traps gdb */
		/* restore stack make gdb know the call stack */
		/* esp must be set prior ebp */
		replay_trap(regs);
	}

	if (syscall_table[regs->orig_eax].replay_handler == NULL) {
		INJ_FATAL("no such syscall post-handler: %d\n", regs->orig_eax);
		replay_trap(regs);
	}

	if (regs->orig_eax == __NR_exit_group) {
		/* it is special. if exit break by signal (that shouldn't happen, but
		 * our inject code may), it never return. */
		syscall_table[regs->orig_eax].replay_handler(regs);
	}

	/* if the syscall use pre_handler, then it need to check signal itself. */
	/* WRONG: now pre_handlers are not allow to write anything to log.  */

	/* we check the signal flag for those syscall. if we read a -1,
	 * there's no signal happen. else, we let user switch ckpt */
	int16_t f = read_int16();
	if (f == -1) {
		/* no signal happen */
		return (uint32_t)(syscall_table[regs->orig_eax].replay_handler(regs));
	} else {
		/* the syscall has been interrupted by signam - f - 1 */
		if (-f - 1 < K_NSIG) {
			INJ_FORCE("signal %d raise in syscall %d\n", -f - 1, nr);
		} else {
			INJ_FATAL("corrupted logger: signal flag is 0x%x\n", (uint16_t)f);
			__exit(-1);
		}

		/* never return */
		replay_sighandler(-f - 1, regs);
#if 0
		/* read the lase 2 bytes to get the signumber */
		seek_logger(-2, SEEK_END);
		int16_t sig = read_int16();
		INJ_FORCE("process distrubed by signal %d, "
				"this logger has over. switch a ckpt.\n", -sig);
		replay_trap(regs);
#endif
		return 0;
	}
}

static void ATTR(unused)
do_make_checkpoint(int ckpt_fd, int maps_fd, int cmdline_fd, int environ_fd,
		struct syscall_regs * r, struct i387_fxsave_struct * fpustate,
		struct seg_regs * seg_regs)
{
	uint32_t f_pos = 0;

	/* write the magic */
	uint32_t magic = CKPT_MAGIC;
	__write(ckpt_fd, &magic, sizeof(magic));
	f_pos += sizeof(magic);

	/* write cmdline */
	char str_buf[512];
	{
		int sz;
		do {
			sz = __read(cmdline_fd, str_buf, 512);
			f_pos += sz;
			__write(ckpt_fd, str_buf, sz);
		} while(sz > 0);

		/* write a zero */
		uint8_t zero = '\0';
		__write(ckpt_fd, &zero, sizeof(zero));
		f_pos += sizeof(zero);
	}

	/* write environ */
	{
		int sz;
		do {
			sz = __read(environ_fd, str_buf, 512);
			f_pos += sz;
			__write(ckpt_fd, str_buf, sz);
		} while(sz > 0);

		/* write a zero */
		uint8_t zero = '\0';
		__write(ckpt_fd, &zero, sizeof(zero));
		f_pos += sizeof(zero);
	}

	/* before we write mem regions, we first set all fields in state_vector, so
	 * when we reload this ckpt, registers is valid in target, not only in the
	 * loader.
	 * */

	/* write registers */
	/* we need a user_regs_struct */
	/* use the regs in state_vector, this is used
	 * only in ckpt */
	struct user_regs_struct * s = &state_vector.regs;
	s->ebx = r->ebx;
	s->ecx = r->ecx;
	s->edx = r->edx;
	s->esi = r->esi;
	s->edi = r->edi;
	s->ebp = r->ebp;
	s->eax = r->eax;
	s->orig_eax = r->orig_eax;
	s->eip = r->eip;
	s->eflags = r->flags;
	s->esp = r->esp;
	/* 6 seg regs */
	if (seg_regs == NULL) {
#define loadsr(d, r) asm volatile("movl %%" #r ", %%eax" : "=a" (d))
		loadsr(s->cs, cs);
		loadsr(s->ds, ds);
		loadsr(s->es, es);
		loadsr(s->fs, fs);
		loadsr(s->gs, gs);
		loadsr(s->ss, ss);
#undef loadsr
	} else {
#define loadsr(d, r) d = seg_regs->r
		loadsr(s->cs, cs);
		loadsr(s->ds, ds);
		loadsr(s->es, es);
		loadsr(s->fs, fs);
		loadsr(s->gs, gs);
		loadsr(s->ss, ss);
#undef loadsr
	}

	INJ_TRACE("eax in ckpt: 0x%x\n", s->eax);
	INJ_TRACE("ebx in ckpt: 0x%x\n", s->ebx);
	INJ_TRACE("ecx in ckpt: 0x%x\n", s->ecx);
	INJ_TRACE("edx in ckpt: 0x%x\n", s->edx);
	INJ_TRACE("esi in ckpt: 0x%x\n", s->esi);
	INJ_TRACE("edi in ckpt: 0x%x\n", s->edi);
	INJ_TRACE("esp in ckpt: 0x%x\n", s->esp);
	INJ_TRACE("ebp in ckpt: 0x%x\n", s->ebp);
	INJ_TRACE("eip in ckpt: 0x%x\n", s->eip);

	/* write state vector */
	/* before we write the vector, we call brk, to make sure each
	 * ckpt file contain heap info */
	state_vector.brk = INTERNAL_SYSCALL(brk, 1, 0);
	
	/* save fpustate */
	if (fpustate == NULL) {
		struct {
			struct i387_fxsave_struct _fpustate;
			uint8_t __padding[16];
		} __attribute__((packed)) align;
#define UL(x)		((uint32_t)(x))
#define ALIGN(x, n)	(void*)(((UL(x)) + (1UL << (n)) - 1) & (~((1UL << (n)) - 1)))
#define ALIGN16(x)	(void*)ALIGN(x, 4)
		struct i387_fxsave_struct * p = ALIGN16(&(align._fpustate));
		save_i387(p);
		fpustate = p;
	}
	memcpy(&state_vector.fpustate, fpustate, sizeof(*fpustate));

	/* we write state_vector again for the loader, before mem regions */
	__write(ckpt_fd, &state_vector, sizeof(state_vector));
	f_pos += sizeof(state_vector);

	/* write mem regions */
	/* readline -1 means reset */
	readline(-1);
	char * p = readline(maps_fd);
	while (p != NULL) {
		void * start, * end;
		int l = -1;
		uint32_t offset;
		char prot[5]="XXXX\0";
		int err;

		INJ_TRACE("%s\n", p);

		sscanf(p, "%lx-%lx %4s %x %*2x:%*2x %*d %n",
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
		/* even if no fn, we write a '\0' info ckpt */
		if (l > 0)
			m.fn_len = strlen(p + l) + 1;
		else
			m.fn_len = 1;

		/* Write the size of m */
		uint32_t sz_m = sizeof(m);
		sz_m += m.fn_len;

		__write(ckpt_fd, &sz_m, sizeof(sz_m));
		f_pos += sizeof(sz_m);

		m.f_pos = f_pos + sz_m;

		/* Write m */
		__write(ckpt_fd, &m, sizeof(m));
		f_pos += sizeof(m);
		if (l > 0) {
			__write(ckpt_fd, p + l, m.fn_len);
		} else {
			char z = '\0';
			__write(ckpt_fd, &z, 1);
		}

		f_pos += m.fn_len;

		/* don't save vdso. in some old system, vdso is mapped at the
		 * top of mem space */
		if (strncmp("[vdso]", p + l, 6) == 0) {
			p = readline(maps_fd);
			continue;
		}

		/* Write memory */
		/* if the region is not writable, use mprotect to reset the PROT */
		if (m.prot & PROT_READ) {
			err = __write(ckpt_fd, start, end-start);
			if (err != end - start) {
				INJ_WARNING("write memregion 0x%x-0x%x failed: err=%d\n",
						start, end, err);
			}
		} else {
			uint32_t old_prot = m.prot;
			uint32_t new_prot = old_prot | PROT_READ;
			INJ_TRACE("this is a unreadable mem region, make it readable now\n");
			err = INTERNAL_SYSCALL(mprotect, 3, start, end - start, new_prot);
			ASSERT(err == 0, r, "mprotect error: %d\n", err);

			err = __write(ckpt_fd, start, end-start);
			if (err != end - start) {
				INJ_WARNING("write memregion failed: err=%d\n", err);
			}

			err = INTERNAL_SYSCALL(mprotect, 3, start, end - start, old_prot);
			ASSERT(err == 0, r, "mprotect error: %d\n", err);
		}


		f_pos += end - start;

		p = readline(maps_fd);
	}

	/* write zero, no more memory regions */
	uint32_t zero = 0;
	__write(ckpt_fd, &zero, sizeof(zero));

	/* reset logger_sz */
	logger_sz = 0;
}
#endif

#ifdef IN_INJECTOR
static void
get_fns(char * logger_fn, char * ckpt_fn, int32_t pid,
		struct syscall_regs * r)
{
	/* get the timeval */
	int err;
	struct timeval tv;
	int i = 0;

	err = INTERNAL_SYSCALL(gettimeofday, 2, &tv, NULL);
	ASSERT(err == 0, r, "gettimeofday failed: %d\n", err);

	snprintf(logger_fn, 128, LOGGER_DIRECTORY"/%d-%u-%u.log",
			pid, tv.tv_sec, tv.tv_usec);
	snprintf(ckpt_fn, 128, LOGGER_DIRECTORY"/%d-%u-%u.ckpt",
			pid, tv.tv_sec, tv.tv_usec);

	do {
		/* check whether the file exists */
		struct stat st;
		int err1, err2;

		err1 = INTERNAL_SYSCALL(stat, 2, logger_fn, &st);
		if (err1 == -ENOENT) {
			err2 = INTERNAL_SYSCALL(stat, 2, ckpt_fn, &st);
			if (err2 == -ENOENT)
				break;
		}
		ASSERT(err1 == 0, r, "stat %s failed: %d\n", err1);
		ASSERT(err2 == 0, r, "stat %s failed: %d\n", err2);

		i++;
		snprintf(logger_fn, 128, LOGGER_DIRECTORY"/%d-%u-%u-%d.log",
				pid, tv.tv_sec, tv.tv_usec, i);
		snprintf(ckpt_fn, 128, LOGGER_DIRECTORY"/%d-%u-%u-%d.ckpt",
				pid, tv.tv_sec, tv.tv_usec, i);
	} while (i < 100);
	ASSERT(i < 100, r,
			"that shouldn't happen: more than 100 ckpts are generated in 1ms\n");
	return;
}
#endif

/* if keep_old is true, we create new checkpoint, don't remove old one.
 * else, we delete old log and ckpt. */
SCOPE void
make_checkpoint(struct syscall_regs * r,
		struct i387_fxsave_struct * fpustate,
		struct seg_regs * seg_regs,
		int keep_old)
{
#ifdef IN_INJECTOR
	char new_logger_fn[128], new_ckpt_fn[128];
	int32_t pid;
	
	/* pid must be set early, we may fork() later, we may fork()
	 * early and we are the child */
	state_vector.pid = pid = INTERNAL_SYSCALL(getpid, 0);

	/* we get new logger fn and ckpt fn */
	get_fns(new_logger_fn, new_ckpt_fn, pid, r);

	/* according to keep_old, we remove or rename the logger */
	if (!keep_old) {
		if (logger_filename[0] != '\0') {
			if (logger_fd > 0)
				INTERNAL_SYSCALL(close, 1, logger_fd);
			INTERNAL_SYSCALL(unlink, 1, logger_filename);
		}
		if (ckpt_filename[0] != '\0')
			INTERNAL_SYSCALL(unlink, 1, ckpt_filename);
	}

	strncpy(logger_filename, new_logger_fn, 128);
	strncpy(ckpt_filename, new_ckpt_fn, 128);

	int maps_fd, cmdline_fd, ckpt_fd, environ_fd;

	/* we open the checkpoint file first, in father process.
	 * if the child scheduled out-of-order, this arrangement makes
	 * the ckpt file be delete correctly. */
	ckpt_fd = INTERNAL_SYSCALL(open, 3,
			new_ckpt_fn, O_WRONLY|O_CREAT|O_TRUNC, 0664);
	ASSERT(ckpt_fd > 0, r, "open ckpt file failed: %d\n", ckpt_fd);

	/* don't use normal fork. */
	/* we don't set SIGCHLD in the 1st param. */
#ifdef FORK_CKPT
	int32_t new_pid = INTERNAL_SYSCALL(clone, 5,
			0, 0, NULL, NULL, NULL);
	ASSERT(new_pid >= 0, r, "clone faild: %d\n", new_pid);

	if (new_pid == 0) {
#endif
		maps_fd = INTERNAL_SYSCALL(open, 2,
				"/proc/self/maps", O_RDONLY);
		ASSERT(maps_fd > 0, r, "open self maps failed: %d", maps_fd);

		cmdline_fd = INTERNAL_SYSCALL(open, 2,
				"/proc/self/cmdline", O_RDONLY);
		ASSERT(cmdline_fd > 0, r, "open self cmdline failed: %d", cmdline_fd);

		environ_fd = INTERNAL_SYSCALL(open, 2,
				"/proc/self/environ", O_RDONLY);
		ASSERT(environ_fd > 0, r, "open self environ failed: %d", environ_fd);

		do_make_checkpoint(ckpt_fd, maps_fd, cmdline_fd, environ_fd, r,
				fpustate, seg_regs);

		INTERNAL_SYSCALL(close, 1, maps_fd);
		INTERNAL_SYSCALL(close, 1, cmdline_fd);
		INTERNAL_SYSCALL(close, 1, environ_fd);
#ifdef FORK_CKPT
		INTERNAL_SYSCALL(exit, 1, 0);
	}
#endif

	INTERNAL_SYSCALL(close, 1, ckpt_fd);
	/* We open new logger file and dup the fd */
	logger_sz = 0;

	int fd = INTERNAL_SYSCALL(open, 3, logger_filename, O_CREAT|O_WRONLY|O_APPEND, 0664);
	ASSERT(fd > 0, r, "open logger failed: %d\n", fd);

	int err = INTERNAL_SYSCALL(dup2, 2, fd, LOGGER_FD);;
	ASSERT(err == LOGGER_FD, &r, "dup2 failed: %d\n", err);

	err = INTERNAL_SYSCALL(close, 1, fd);
	ASSERT(err == 0, &r, "close failed: %d\n", err);

	logger_fd = LOGGER_FD;

#endif
}

// vim:ts=4:sw=4

