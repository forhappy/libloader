/* 
 * ptrace.c
 * by WN @ Apr. 19, 2010
 */


#include <host/ptrace.h>
#include <host/exception.h>
#include <host/mm.h>
#include <common/debug.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/personality.h>
/* some very old systems' include file doesn't contain this symbol */
#define ADDR_NO_RANDOMIZE (0x0040000)
#include <linux/ptrace.h>
#include <stdarg.h>
#include <signal.h>

/* wait for the chld, and check whether it is exited*/
static int
wait_and_check(pid_t pid)
{
	siginfo_t si;
	int err;
	TRACE(PTRACE, "wait...\n");
	signal(SIGINT, SIG_IGN);
	err = waitid(P_PID, pid, &si, WEXITED | WSTOPPED);
	signal(SIGINT, SIG_DFL);
	TRACE(PTRACE, "wait over\n");

	if (err < 0)
		THROW_FATAL(EXP_PTRACE, "wait failed: %s", strerror(errno));

	/* Check for siginfo */
	if (si.si_code != CLD_TRAPPED) {
		ptrace(PTRACE_KILL, pid, NULL, NULL);
		FORCE(PTRACE, "signal: %d\n", si.si_status);
		THROW_FATAL(EXP_PTRACE, "Child process %d has stopped or been killed (%d)",
				pid, si.si_code);
	}
	SILENT(PTRACE, "code=%d, status=%d\n",si.si_code, si.si_status);
	return si.si_status;
}

void
ptrace_kill(pid_t pid)
{
	TRACE(PTRACE, "kill %d\n", pid);
	ptrace(PTRACE_KILL, pid, NULL, NULL);
}

pid_t
ptrace_execve(char ** argv, char ** env, char * exec_fn)
{
	int err;

	assert(exec_fn != NULL);
	assert(argv != NULL);
	assert(env != NULL);

	TRACE(PTRACE, "exec_fn: %s\n", exec_fn);

	catch_var(pid_t, pid, -1);
	define_exp(exp);
	TRY(exp) {
		set_catched_var(pid, fork());
		if (pid < 0)
			THROW_FATAL(EXP_PTRACE, "fork failed: %s\n", strerror(errno));
		if (pid == 0) {
			/* child process */
			err = personality(0xffffffffUL);
			err = personality(err | ADDR_NO_RANDOMIZE);
			assert(err != -1);

			err = ptrace(PTRACE_TRACEME, getpid(), NULL, NULL);
			CASSERT(PTRACE, err != -1, "traceme failed: %s", strerror(errno));

			err = execve(exec_fn, argv, env);
			/* we shouldn't get here! */
			assert(0);
		}

		/* parent process */
		wait_and_check(pid);

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
		err = ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
		if (err == -1)
			THROW_FATAL(EXP_PTRACE, "ptrace_setoptions failed: %s",
					strerror(errno));
	} FINALLY {
	} CATCH(exp) {
		get_catched_var(pid);
		if ((pid != -1) && (pid != 0))
			ptrace(PTRACE_KILL, pid, NULL, NULL);
		RETHROW(exp);
	}
	return pid;
}

void
ptrace_dupmem(pid_t pid, void * dst, uintptr_t addr, int len)
{
	assert(dst != NULL);

#if 0
	assert(child_pid != -1);
	assert(dst != NULL);

	uintptr_t target_start, target_end;
	target_start = (addr + 3) & 0xfffffffcul;
	target_end = (addr + len) & 0xfffffffcul;

	uintptr_t target_ptr = target_start;
	uint32_t * dst_ptr = (uint32_t*)(dst + (target_start - addr));

	while (target_ptr < target_end) {
		long val;
		val = ptrace(PTRACE_PEEKDATA, child_pid, target_ptr, NULL);
		ETHROW("ptrace peek data failed: %s", 
				strerror(errno));
		(*(uint32_t*)(dst_ptr)) = val;
		dst_ptr ++;
		target_ptr += 4;
	}

	if (target_start != addr) {
		/* head fragment */
		uintptr_t target_frag = addr & 0xfffffffcul;
		long val = ptrace(PTRACE_PEEKDATA, child_pid, target_frag, NULL);
		uint8_t * ptr = (uint8_t*)&val;
		int i = addr - target_frag;
		int j = 0;
		for (; (i < 4) && (j < len); i++, j++)
			((uint8_t*)dst)[j] = ptr[i];
	}

	if (target_end != addr + len) {
		if (((addr + len) & 0xfffffffcul) >= addr) {
			/* tail fragment */
			uintptr_t tail_frag = target_end;
			long val = ptrace(PTRACE_PEEKDATA, child_pid, tail_frag, NULL);
			uint8_t * ptr = (uint8_t*)&val;
			int i = 0;
			int j = target_end - addr;
			for (; j < len; i++, j++) {
				if (j < 0)
					continue;
				((uint8_t*)dst)[j] = ptr[i];
			}
		}
	}
	return;
#endif
}


// vim:ts=4:sw=4

