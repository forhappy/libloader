/* 
 * ptraceutils.c
 * by WN @ Jun. 04, 2009
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/personality.h>
#include <linux/user.h>


#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"

static pid_t child_pid = -1;

static void
ptrace_cleanup(struct cleanup * _s)
{
	remove_cleanup(_s);
	/* At least, kill the child process */
	ptrace(PTRACE_KILL, child_pid, NULL, NULL);
	child_pid = -1;
}

static struct cleanup pt_clup_s = {
	.function = ptrace_cleanup,
};

/* wait for the chld, and check whether it is exited*/
static void
wait_and_check(void)
{
	siginfo_t si;
	int err;
	err = waitid(P_PID, child_pid, &si, WEXITED | WSTOPPED);
	if (err < 0) {
		THROW(EXCEPTION_FATAL, "wait failed");
	}
	/* Check for siginfo */
	if (si.si_code != CLD_TRAPPED) {
		int old_pid = child_pid;
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);
		child_pid = -1;
		THROW(EXCEPTION_FATAL, "Child process %d has stopped or been killed (%d)", old_pid,
				si.si_code);
	}
	return;
}

void
ptrace_execve(const char * filename,
		char ** argv)
{
	int err;

	assert(filename != NULL);
	TRACE(PTRACE, "prepare to execve file %s\n", filename);
	
	child_pid = fork();
	if (child_pid < 0)
		THROW(EXCEPTION_FATAL, "fork failed: %s", strerror(errno));
	if (child_pid == 0) {
		/* in child process */

		/* set personality */
		err = personality(0xffffffffUL);
		err = personality(err | ADDR_NO_RANDOMIZE);
		assert(err != -1);

		err = ptrace(PTRACE_TRACEME, getpid(), NULL, NULL);
		if (err == -1)
			THROW(EXCEPTION_FATAL, "ptrace error: %s", strerror(errno));

		err = execve(filename, argv, environ);
		THROW(EXCEPTION_FATAL, "execve failed: %s", strerror(errno));
	}

	/* in father process */
	make_cleanup(&pt_clup_s);
	wait_and_check();
	return;
}

void
ptrace_dupmem(void * dst, uintptr_t addr, int len)
{
	assert(child_pid != -1);
	assert(addr % 4 != 0);
	assert(len % 4 != 0);

	int i;
	for (i = 0; i < len; i += 4) {
		long val;
		val = ptrace(PTRACE_PEEKDATA, child_pid, addr + i, NULL);
		if ((val == -1) && (errno != 0)) {
			THROW(EXCEPTION_FATAL, "ptrace peek data failed: %s",
					strerror(errno));
		}
		*((uint32_t*)(dst + i)) = val;
	}
	return;
}

void
ptrace_kill(void)
{
	ptrace_cleanup(&pt_clup_s);
}

void
ptrace_detach(bool_t wait)
{
	int err;
	pid_t old_pid = child_pid;
	err = ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
	if (err != 0)
		THROW(EXCEPTION_FATAL, "cannot detach: %s", strerror(errno));
	remove_cleanup(&pt_clup_s);
	/* wait for the process */
	if (!wait)
		return;

	siginfo_t si;
	err = waitid(P_PID, old_pid, &si, WEXITED);
	if (err == -1) {
		ERROR(PTRACE, "wait error: %s\n", strerror(errno));
	} else {
		TRACE(PTRACE, "target process finished\n");
	}
	return;
}


// vim:tabstop=4:shiftwidth=4

