/* 
 * ptrace.c
 * by WN @ Apr. 19, 2010
 */


#include <host/ptrace.h>
#include <host/exception.h>
#include <host/mm.h>
#include <common/debug.h>
#include <common/defs.h>

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

	CTHROW_FATAL(err >= 0, EXP_PTRACE, "wait failed: %s", strerror(errno));

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

static void
do_execve(char ** argv, char ** env, char * exec_fn)
{
	int err;
	err = personality(0xffffffffUL);
	err = personality(err | ADDR_NO_RANDOMIZE);
	assert(err != -1);

	err = ptrace(PTRACE_TRACEME, getpid(), NULL, NULL);
	CASSERT(PTRACE, errno == 0, "traceme failed: %s", strerror(errno));

	err = execve(exec_fn, argv, env);
	/* we shouldn't get here! */
	assert(0);
}

static void
do_parent_execve(char ** argv, char ** env, char * exec_fn)
{
	int err;
	err = personality(0xffffffffUL);
	err = personality(err | ADDR_NO_RANDOMIZE);
	assert(err != -1);

	/* don't set TRACEME here! */

	err = execve(exec_fn, argv, env);
	/* we shouldn't get here! */
	assert(0);
}


pid_t
ptrace_execve(char ** argv, char ** env, char * exec_fn,
		bool_t parent_execve)
{
	int err;

	assert(exec_fn != NULL);
	assert(argv != NULL);
	assert(env != NULL);

	TRACE(PTRACE, "exec_fn: %s\n", exec_fn);

	pid_t parent_pid = getpid();
	catch_var(pid_t, pid, -1);
	define_exp(exp);
	TRY(exp) {
		pid_t child_pid;
		child_pid = fork();
		CTHROW_FATAL(child_pid >= 0, EXP_PTRACE, "fork failed: %s\n",
				strerror(errno));
		if (!parent_execve) {
			if (child_pid == 0) {
				/* child process */
				do_execve(argv, env, exec_fn);
			} else {
				/* parent needn't do anything */
				set_catched_var(pid, child_pid);
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
				CTHROW_FATAL(err != -1, EXP_PTRACE, "ptrace_setoptions failed: %s",
						strerror(errno));
			}
		} else {
			/* 
			 * the family of processes when parent_execve
			 *
			 * (./snitchaser) ---------(sigchld)--(loop)-->
			 *                |            ^       ^
			 *                |            |       |
			 *                --tmp---->(exit 0)   |
			 *                      |              |
			 *                      |              |
			 *                      ------------(attach)-->
			 *
			 * Another possibility:
			 *
			 * (./snitchaser) -----------------(sigchld)--(loop)-->
			 *                |           ^        ^
			 *                |           |        |
			 *                |           |        |
			 *                |           |        |
			 *                +-tmp-+-----+---->(exit 0) 
			 *                      |     |        
			 *                      |     |        
			 *                      +--(attach)-->
			 *
			 * */
			volatile int parent_execve_lock = 0;
			if (child_pid != 0) {
				/* wait child finish */
				waitpid(child_pid, NULL, 0);
				/* iterate over parent_execve_lock */
				while (parent_execve_lock == 0);
				do_parent_execve(argv, env, exec_fn);
			} else {

				/* reparent child */
				{
					pid_t tmp = fork();
					assert(tmp >= 0);
					if (tmp != 0) {
						exit(0);
					}
				}

				set_catched_var(pid, parent_pid);
				err = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
				CTHROW_FATAL(err != -1, EXP_PTRACE, "child process %d "
						"is unable to attach to parent process %d\n",
						getpid(), parent_pid);
				wait_and_check(pid);

				err = ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
				CTHROW_FATAL(err != -1, EXP_PTRACE, "ptrace_setoptions failed: %s",
						strerror(errno));

				/* set parent_execve_lock to 1 */
				parent_execve_lock = 1;
				ptrace_updmem(pid, (void*)&parent_execve_lock,
						(uintptr_t)(&parent_execve_lock), sizeof(parent_execve_lock));

				while (1) {
					/* wait for parent to call 'execve' */
					err = ptrace(PTRACE_CONT, pid, NULL, NULL);
					CTHROW_FATAL(err != -1, EXP_PTRACE, "PTRACE_CONT failed\n");

					int status = wait_and_check(pid);
					/* see the graph above */
					if (status == SIGCHLD)
						continue;
					CTHROW_FATAL(status == SIGTRAP, EXP_PTRACE, "receives %d from %d, "
							"not SIGTRAP(%d)\n", status, pid, SIGTRAP);
					break;
				}
			}
		}
	} FINALLY {
	} CATCH(exp) {
		get_catched_var(pid);
		if ((pid != -1) && (pid != 0)) {
			if (pid != parent_pid) {
				ptrace(PTRACE_KILL, pid, NULL, NULL);
			} else {
				WARNING(PTRACE, "killing parent process\n");
				kill(parent_pid, SIGKILL);
				kill(parent_pid, SIGCONT);
			}
		}
		RETHROW(exp);
	}
	return pid;
}

void
ptrace_dupmem(pid_t pid, void * dst, uintptr_t addr, size_t len)
{
	assert(dst != NULL);

#define LONGSZ	sizeof(long)
	uintptr_t target_start, target_end;
	target_start = ALIGN_DOWN_PTR(addr, LONGSZ);
	target_end = ALIGN_UP_PTR(addr + len, LONGSZ);

	int tmp_sz = target_end - target_start;
	assert(tmp_sz >= (int)LONGSZ);

	define_exp(exp);
	catch_var(void *, tmp_ptr, NULL);
	TRY(exp) {
		set_catched_var(tmp_ptr, xmalloc(target_end - target_start));
		uintptr_t target_ptr = target_start;
		long * dst_ptr = tmp_ptr;
		errno = 0;
		while (target_ptr < target_end) {
			long val;
			val = ptrace(PTRACE_PEEKDATA, pid, target_ptr, NULL);
			ETHROW_FATAL(EXP_PTRACE, "peek data failed: %s", strerror(errno));
			*dst_ptr = val;
			dst_ptr ++;
			target_ptr += LONGSZ;
		}
		memcpy(dst, tmp_ptr + (addr - target_start), len);
	} FINALLY {
		get_catched_var(tmp_ptr);
		xfree(tmp_ptr);
	} CATCH(exp) {
		RETHROW(exp);
	}
}

void
ptrace_updmem(pid_t pid, const void * src, uintptr_t addr, size_t len)
{
	assert(pid > 0);
	assert(src != NULL);
	if (len == 0)
		return;

	const void * src_end = src + len;

	errno = 0;
	/* first, get the head fragment and upload data */
	if (addr % LONGSZ != 0) {
		/* we have head fragment */
		uintptr_t frag_word = ALIGN_DOWN_PTR(addr, LONGSZ);
		int offset = addr % LONGSZ;
		/* dup the frag word */
		long val = ptrace(PTRACE_PEEKDATA, pid, frag_word, NULL);
		ETHROW_FATAL(EXP_PTRACE, "peek data failed: %s", strerror(errno));

		/* fill fragment */
		int cp_len = min(len, LONGSZ - offset);
		memcpy((void*)(&val) + offset, src, cp_len);

		/* push back */
		ptrace(PTRACE_POKEDATA, pid, frag_word, val);
		ETHROW_FATAL(EXP_PTRACE, "poke data failed: %s", strerror(errno));

		/* adjust src and addr */
		src += cp_len;
		addr += cp_len;
		len -= cp_len;
	}

	if (len == 0)
		return;

	assert(addr % LONGSZ == 0);

	/* then, fill its main contents */
	uintptr_t from = addr;
	uintptr_t to = ALIGN_DOWN_PTR(addr + len, LONGSZ);
	uintptr_t ptr = from;
	while (ptr < to) {
		long val = *((long *)(src));
		ptrace(PTRACE_POKEDATA, pid, ptr, val);
		ETHROW_FATAL(EXP_PTRACE, "poke data failed: %s", strerror(errno));
		src += LONGSZ;
		ptr += LONGSZ;
		len -= LONGSZ;
	}

	/* finally, fill the tail fragment */
	if (src != src_end) {
		assert(src < src_end);
		/* peek frag */
		/* dup the frag word */
		long val = ptrace(PTRACE_PEEKDATA, pid, ptr, NULL);
		ETHROW_FATAL(EXP_PTRACE, "peek data failed: %s", strerror(errno));
		memcpy(&val, src, src_end - src);
		/* poke val back */
		ptrace(PTRACE_POKEDATA, pid, ptr, val);
		ETHROW_FATAL(EXP_PTRACE, "poke data failed: %s", strerror(errno));
	}
	return;
}

uintptr_t
ptrace_push(pid_t pid, const void * data, size_t len)
{
	errno = 0;
	uintptr_t esp = ptrace(PTRACE_PEEKUSER, pid,
			(void*)(offsetof(struct user_regs_struct, esp)), NULL);
	ETHROW_FATAL(EXP_PTRACE, "peekuser failed");

	size_t real_len = ALIGN_UP(len, 4);
	esp -= real_len;
	ptrace_updmem(pid, data, esp, real_len);

	ptrace(PTRACE_POKEUSER, pid,
			(void*)(offsetof(struct user_regs_struct, esp)), esp);
	ETHROW_FATAL(EXP_PTRACE, "reset esp failed\n");
	return esp;
}

void
ptrace_goto(pid_t pid, uintptr_t eip)
{
	ptrace(PTRACE_POKEUSER, pid,
			(void*)offsetof(struct user_regs_struct, eip),
			eip);
	ETHROW_FATAL(EXP_PTRACE, "error in setting eip: %s",
			strerror(errno));
	return;
}

void
ptrace_detach(pid_t pid)
{
	ptrace(PTRACE_DETACH, pid, NULL, NULL);
	ETHROW_FATAL(EXP_PTRACE, "cannot detach: %s", strerror(errno));
}

// vim:ts=4:sw=4

