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
#include <linux/ptrace.h>
#include <stdarg.h>
#include <signal.h>

#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"

static pid_t child_pid = -1;

static void
ptrace_cleanup(struct cleanup * _s)
{
	remove_cleanup(_s);
	/* At least, kill the child process */
	if (child_pid != -1) {
		TRACE(PTRACE, "kill child %d\n", child_pid);
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);
		child_pid = -1;
	}
}

static struct cleanup pt_clup_s = {
	.function = ptrace_cleanup,
};


/* wait for the chld, and check whether it is exited*/
static int
wait_and_check(void)
{
	siginfo_t si;
	int err;
	TRACE(PTRACE, "wait...\n");
	signal(SIGINT, SIG_IGN);
	err = waitid(P_PID, child_pid, &si, WEXITED | WSTOPPED);
	signal(SIGINT, SIG_DFL);
	TRACE(PTRACE, "wait over\n");

	CTHROW(err >= 0, "wait failed");

	/* Check for siginfo */
	if (si.si_code != CLD_TRAPPED) {
		int old_pid = child_pid;
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);
		child_pid = -1;
		FORCE(PTRACE, "signal: %d\n", si.si_status);
		THROW(EXCEPTION_FATAL, "Child process %d has stopped or been killed (%d)", old_pid,
				si.si_code);
	}
	SILENT(SYSTEM, "code=%d, status=%d\n",si.si_code, si.si_status);
	return si.si_status;
}

pid_t
ptrace_execve(const char * filename,
		char ** argv, char ** environ)
{
	int err;

	assert(filename != NULL);
	TRACE(PTRACE, "prepare to execve file %s\n", filename);
	
	child_pid = fork();
	CTHROW(child_pid >= 0, "fork failed: %s", strerror(errno));
	if (child_pid == 0) {
		/* in child process */

		/* set personality */
		err = personality(0xffffffffUL);
		err = personality(err | ADDR_NO_RANDOMIZE);
		assert(err != -1);

		err = ptrace(PTRACE_TRACEME, getpid(), NULL, NULL);
		CTHROW(err != -1, "traceme failed: %s", strerror(errno));

		err = execve(filename, argv, environ);
		THROW(EXCEPTION_FATAL, "execve failed: %s", strerror(errno));
	}

	/* in father process */
	make_cleanup(&pt_clup_s);

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
	CTHROW(err != -1, "ptrace_setoptions failed: %s", strerror(errno));
	return child_pid;
}

void
ptrace_dupmem(void * dst, uintptr_t addr, int len)
{
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
}

void
ptrace_updmem(const void * src, uintptr_t addr, int len)
{
	assert(child_pid != -1);
	uintptr_t target_start, target_end;
	target_start = (addr + 3) & 0xfffffffcul;
	target_end = (addr + len) & 0xfffffffcul;

	uintptr_t target_ptr = target_start;
	uint32_t * src_ptr = (uint32_t*)(src + (target_start - addr));

	while (target_ptr < target_end) {
		long val;
		val = ptrace(PTRACE_POKEDATA, child_pid, target_ptr,
				*src_ptr);
		ETHROW("ptrace poke data to 0x%x failed: %s", target_ptr,
				strerror(errno));
		src_ptr ++;
		target_ptr += 4;
	}

	if (target_start != addr) {
		/* head fragment */
		uintptr_t target_frag = addr & 0xfffffffcul;
		long val = ptrace(PTRACE_PEEKDATA, child_pid, target_frag, NULL);
		ETHROW("ptrace peek data tailed: %s",
				strerror(errno));
		uint8_t * ptr = (uint8_t*)&val;
		int i = addr - target_frag;
		int j = 0;
		for (; (i < 4) && (j < len); i++, j++)
			ptr[i] = ((uint8_t*)src)[j];
		/* poke the word back */
		ptrace(PTRACE_POKEDATA, child_pid, target_frag, val);
		ETHROW("ptrace poke data tailed: %s",
				strerror(errno));
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
				ptr[i] = ((uint8_t*)src)[j];
			}
			/* poke the word back */
			ptrace(PTRACE_POKEDATA, child_pid, tail_frag, val);
			ETHROW("ptrace poke data tailed: %s",
					strerror(errno));
		}
	}
	return;
}

void
ptrace_kill(void)
{
	ptrace_cleanup(&pt_clup_s);
}


/* the loader proc run as a signal proxy,
 * when it receive a signal, it pass it to it child */
static void
signal_proxy(int signum)
{
	if (child_pid != 0)
		kill(child_pid, signum);
}

void
ptrace_detach(bool_t wait)
{
	int err;
	pid_t old_pid = child_pid;
	err = ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
	ETHROW("cannot detach: %s", strerror(errno));
	remove_cleanup(&pt_clup_s);
	/* wait for the process */
	if (!wait)
		return;

	/* we install our signal proxy */
	for (int i = 1; i <= SIGRTMAX; i++) {
		signal(i, signal_proxy);
	}

	/* when we wait child, ignore the SIGINT signal */
	signal(SIGINT, SIG_IGN);

	siginfo_t si;
	for (;;) {
		errno = 0;
		err = waitid(P_ALL, old_pid, &si, WEXITED);
		if (err == -1) {
			if (errno != EINTR) {
				THROW(EXCEPTION_FATAL, "wait error: %s", strerror(errno));
			}
			/* if not, a signal raise in waitid. our signal proxy should have been
			 * resend the signal to child, we loop again. */
		} else {
			if (si.si_pid != old_pid)
				continue;

			TRACE(PTRACE, "target process finished\n");
			VERBOSE(PTRACE, "target process finished with status: %o\n", si.si_status);

			int status = si.si_status;
			if (WIFEXITED(status)) {
				VERBOSE(PTRACE, "target exited normally, exit code: %d\n",
						WEXITSTATUS(status));
			}
			if (WIFSIGNALED(status)) {
				VERBOSE(PTRACE, "target signaled, term sig: %d\n",
						WTERMSIG(status));
			}
			break;
		}
	}
	return;
}

struct user_regs_struct
ptrace_peekuser(void)
{
	struct user_regs_struct ret;
	memset(&ret, 0, sizeof(ret));
	assert(child_pid != -1);
	errno = 0;

	ptrace(PTRACE_GETREGS, child_pid, NULL, &ret);
	ETHROW("error in peeking regs: %s", strerror(errno));
	return ret;
}

void
ptrace_pokeuser(struct user_regs_struct s)
{
	assert(child_pid != -1);
	errno = 0;
	ptrace(PTRACE_SETREGS, child_pid, NULL, &s);
	ETHROW("error in pokeing regs: %s",
			strerror(errno));
}

#define ARCH_INTINSTR	{'\xcc'}
static uint8_t bkpt_instr[] = ARCH_INTINSTR;
#define INTINSTR_LEN	(sizeof(bkpt_instr))
static uintptr_t current_bkpt = 0;
static uint8_t saved_bkpt_instr[INTINSTR_LEN];

void
ptrace_insert_bkpt(uintptr_t address)
{
	CTHROW(current_bkpt == 0,
			"a breakpoint has already been inserted");
	ptrace_dupmem(saved_bkpt_instr, address, INTINSTR_LEN);
	ptrace_updmem(bkpt_instr, address, INTINSTR_LEN);
	current_bkpt = address;
}

uintptr_t
ptrace_cont(void)
{
	ptrace(PTRACE_CONT, child_pid, NULL,NULL);
	wait_and_check();
	struct user_regs_struct regs = ptrace_peekuser();
	return regs.eip;
}

uintptr_t
ptrace_next_syscall(int (*hook)(struct user_regs_struct u, bool_t before))
{
	TRACE(PTRACE, "run to next syscall\n");
	struct user_regs_struct uregs;
	int status;
	int err;
	
	ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	ETHROW("ptrace pre-syscall failed: %s\n", strerror(errno));
	status = wait_and_check();
	if ((status & 0x80) == 0) {
		/* this is not a syscall, is a trap */
		uregs = ptrace_peekuser();
		SYS_FORCE("syscall hit a trap at uregs.eip\n");
		return uregs.eip;
	}

	if (hook) {
		uregs = ptrace_peekuser();
		err = hook(uregs, TRUE);
		CTHROW(err >= 0, "syscall pre-hook failed: %s", strerror(errno));
	}

	ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	ETHROW("ptrace after-syscall failed: %s\n", strerror(errno));
	wait_and_check();
	if (hook) {
		uregs = ptrace_peekuser();
		err = hook(uregs, FALSE);
		CTHROW(err >= 0, "syscall after-hook failed: %s", strerror(errno));
	}

	struct user_regs_struct regs = ptrace_peekuser();
	return regs.eip;
}

void
ptrace_goto(uintptr_t addr)
{
	ptrace(PTRACE_POKEUSER, child_pid,
			(void*)offsetof(struct user_regs_struct, eip),
			addr);
	ETHROW("error in setting eip: %s",
			strerror(errno));
	return;
}

void
ptrace_resume(void)
{
	if (current_bkpt == 0) {
		WARNING(PTRACE, "no breakpoint to resume\n");
		return;
	}

	ptrace_updmem(saved_bkpt_instr, current_bkpt, INTINSTR_LEN);
	/* change the eip */
	ptrace_goto(current_bkpt);
	current_bkpt = 0;

	return;
}

uint32_t
ptrace_push(const void * data, int len, bool_t save_esp)
{
	uint32_t esp = ptrace(PTRACE_PEEKUSER, child_pid,
			(void*)offsetof(struct user_regs_struct, esp), NULL);
	ETHROW("cannot get esp");

	int err;
	uint32_t real_len = (len + 3) & 0xfffffffcUL;
	esp -= real_len;
	ptrace_updmem(data, esp, len);
	TRACE(PTRACE, "updmemory from 0x%x, len=%d\n", esp, len);

	if (!save_esp) {
		err = ptrace(PTRACE_POKEUSER, child_pid,
			(void*)offsetof(struct user_regs_struct, esp), esp);
		ETHROW("reset esp failed");
	}
	return esp;
}

#ifdef ptrace_syscall
#undef ptrace_syscall
#endif
int ptrace_syscall(int no, int nr, ...)
{
#define SYSCALL_INSTR {'\xcd', '\x80'}
	uint8_t syscall_instr[] = SYSCALL_INSTR;
#define SYSCALL_INSTR_LEN	(sizeof(syscall_instr))
	uint8_t saved_syscall_instr[SYSCALL_INSTR_LEN];

	TRACE(PTRACE, "call syscall %d\n", no);
	struct user_regs_struct regs, saved_regs;
	regs = saved_regs = ptrace_peekuser();

	regs.eax = no;

	va_list ap;
	va_start(ap, nr);
#define setreg(x)	regs.x = va_arg(ap, uint32_t); break
#define xcase(nr, x) case nr: setreg(x)
	for (int i = 0; i < nr; i++) {
		switch (i) {
			xcase(0, ebx);
			xcase(1, ecx);
			xcase(2, edx);
			xcase(3, esi);
			xcase(4, edi);
			xcase(5, ebp);
			default:
				THROW(EXCEPTION_FATAL, "too many syscall args");
		}
	}
#undef xcase
#undef setreg
	va_end(ap);

	ptrace_pokeuser(regs);

	/* insert the int80 instruction */
	ptrace_dupmem(saved_syscall_instr, saved_regs.eip, SYSCALL_INSTR_LEN);
	ptrace_updmem(syscall_instr, saved_regs.eip, SYSCALL_INSTR_LEN);

	/* trace systemcall and continue */
	ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	ETHROW("ptrace syscall failed: %s",
			strerror(errno));
	
	wait_and_check();

	ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	ETHROW("ptrace syscall failed: %s",
			strerror(errno));
	wait_and_check();

	/* restore the int80 command */
	ptrace_updmem(saved_syscall_instr, saved_regs.eip, SYSCALL_INSTR_LEN);

	TRACE(PTRACE, "retrieve retval\n");
	/* retrieve retval */
	int32_t retval = ptrace(PTRACE_PEEKUSER, child_pid,
			offsetof(struct user_regs_struct, eax), NULL);

	/* restore register */
	TRACE(PTRACE, "restore regs\n");
	ptrace_pokeuser(saved_regs);
	return retval;
}

// vim:tabstop=4:shiftwidth=4

