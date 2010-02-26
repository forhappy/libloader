#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/personality.h>
#include <linux/ptrace.h>
#include <stdarg.h>
#include <signal.h>
#include <assert.h>

struct user_regs_struct {
	unsigned long	bx;
	unsigned long	cx;
	unsigned long	dx;
	unsigned long	si;
	unsigned long	di;
	unsigned long	bp;
	unsigned long	ax;
	unsigned long	ds;
	unsigned long	es;
	unsigned long	fs;
	unsigned long	gs;
	unsigned long	orig_ax;
	unsigned long	ip;
	unsigned long	cs;
	unsigned long	flags;
	unsigned long	sp;
	unsigned long	ss;
};


static pid_t child_pid = -1;

static int
wait_and_check(void)
{
	siginfo_t si;
	int err;
	signal(SIGINT, SIG_IGN);
	err = waitid(P_PID, child_pid, &si, WEXITED | WSTOPPED);
	signal(SIGINT, SIG_DFL);

	assert(err >= 0);

	/* Check for siginfo */
	if (si.si_code != CLD_TRAPPED) {
		int old_pid = child_pid;
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);
		child_pid = -1;
		fprintf(stderr, "killed by signal: %d\n", si.si_status);
	}
	return si.si_status;
}

struct user_regs_struct
ptrace_peekuser(void)
{
	struct user_regs_struct ret;
	memset(&ret, 0, sizeof(ret));
	assert(child_pid != -1);
	errno = 0;

	ptrace(PTRACE_GETREGS, child_pid, NULL, &ret);
	return ret;
}

static int
ptrace_next_inst(void)
{
	int err = ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);
	wait_and_check();
}

static void
ptrace_kill(void)
{
	/* At least, kill the child process */
	if (child_pid != -1) {
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);
		child_pid = -1;
	}
}

static pid_t
ptrace_execve(const char * filename,
		char ** argv, char ** environ)
{
	int err;

	assert(filename != NULL);
	child_pid = fork();
	assert(child_pid >= 0);
	if (child_pid == 0) {
		/* in child process */

		/* set personality */
		err = personality(0xffffffffUL);
		err = personality(err | ADDR_NO_RANDOMIZE);
		assert(err != -1);

		err = ptrace(PTRACE_TRACEME, getpid(), NULL, NULL);
		assert(err != -1);

		execve(filename, argv, environ);
		assert(0);
	}

	wait_and_check();

	/*
	 * ptrace(2):
	 * ...
	 * PTRACE_O_TRACESYSGOOD:
	 * When delivering syscall traps, set bit 7 in the signal number (i.e.,
	 * deliver (SIGTRAP | 0x80) This makes it easy for the tracer to tell the
	 * difference between normal traps and those caused by a syscall.
	 * (PTRACE_O_TRACESYSGOOD may not work on all architectures.)
	 * ...
	 * */
	err = ptrace(PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_TRACESYSGOOD);

	assert(err != -1);
	return child_pid;
}


int
main(int argc, char * argv[])
{
	assert(argc >= 2);
	ptrace_execve(argv[1], argv + 1, environ);

	struct user_regs_struct regs;
	int err;
	err = ptrace_next_inst();
	while (err >= 0) {
		regs = ptrace_peekuser();
		printf("0x%lx\n", regs.ip);
		err = ptrace_next_inst();
	}

	ptrace_kill();
	return 0;
}


