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

/* wait for the chld, and check whether it is exited*/
static void
wait_and_check(void)
{
	siginfo_t si;
	waitid(P_PID, child_pid, &si, WEXITED | WSTOPPED);
	/* Check for siginfo */
	if (si.si_code != CLD_STOPPED) {
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);
		THROW(EXCEPTION_FATAL, "Child process %d has stopped or been killed", child_pid);
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
	wait_and_check();
	return;
}


// vim:tabstop=4:shiftwidth=4

