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
#include <stdarg.h>

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
static void
wait_and_check(void)
{
	siginfo_t si;
	int err;
	TRACE(PTRACE, "wait...\n");
	signal(SIGINT, SIG_IGN);
	err = waitid(P_PID, child_pid, &si, WEXITED | WSTOPPED);
	signal(SIGINT, SIG_DFL);
	TRACE(PTRACE, "wait over\n");

	FORCE(PTRACE, "si_code=%d\n", si.si_code);
	FORCE(PTRACE, "si_status=%d\n", si.si_status);




	assert_throw(err >= 0, "wait failed");

	/* Check for siginfo */
	if (si.si_code != CLD_TRAPPED) {
		int old_pid = child_pid;
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);
		child_pid = -1;
		FORCE(PTRACE, "signal: %d\n", si.si_status);
		THROW(EXCEPTION_FATAL, "Child process %d has stopped or been killed (%d)", old_pid,
				si.si_code);
	}
	return;
}

pid_t
ptrace_execve(const char * filename,
		char ** argv)
{
	int err;

	assert(filename != NULL);
	TRACE(PTRACE, "prepare to execve file %s\n", filename);
	
	child_pid = fork();
	assert_throw(child_pid >= 0, "fork failed: %s", strerror(errno));
	if (child_pid == 0) {
		/* in child process */

		/* set personality */
		err = personality(0xffffffffUL);
		err = personality(err | ADDR_NO_RANDOMIZE);
		assert(err != -1);

		err = ptrace(PTRACE_TRACEME, getpid(), NULL, NULL);
		assert_throw(err != -1, "traceme failed: %s", strerror(errno));

		err = execve(filename, argv, environ);
		THROW(EXCEPTION_FATAL, "execve failed: %s", strerror(errno));
	}

	/* in father process */
	make_cleanup(&pt_clup_s);
	wait_and_check();
	return child_pid;
}

void
ptrace_dupmem(void * dst, uintptr_t addr, int len)
{
	assert(child_pid != -1);
	uintptr_t target_start, target_end;
	target_start = (addr + 3) & 0xfffffffcul;
	target_end = (addr + len) & 0xfffffffcul;

	uintptr_t target_ptr = target_start;
	uint32_t * dst_ptr = (uint32_t*)(dst + (target_start - addr));

	while (target_ptr < target_end) {
		long val;
		val = ptrace(PTRACE_PEEKDATA, child_pid, target_ptr, NULL);
		assert_errno_throw("ptrace peek data failed: %s", 
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
ptrace_updmem(void * src, uintptr_t addr, int len)
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
		assert_errno_throw("ptrace poke data failed: %s", 
				strerror(errno));
		src_ptr ++;
		target_ptr += 4;
	}

		assert_errno_throw("intercept: %s",
				strerror(errno));
	if (target_start != addr) {
		/* head fragment */
		uintptr_t target_frag = addr & 0xfffffffcul;
		long val = ptrace(PTRACE_PEEKDATA, child_pid, target_frag, NULL);
		assert_errno_throw("ptrace peek data tailed: %s",
				strerror(errno));
		uint8_t * ptr = (uint8_t*)&val;
		int i = addr - target_frag;
		int j = 0;
		for (; (i < 4) && (j < len); i++, j++)
			ptr[i] = ((uint8_t*)src)[j];
		/* poke the word back */
		ptrace(PTRACE_POKEDATA, child_pid, target_frag, val);
		assert_errno_throw("ptrace poke data tailed: %s",
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
			assert_errno_throw("ptrace poke data tailed: %s",
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

void
ptrace_detach(bool_t wait)
{
	int err;
	pid_t old_pid = child_pid;
	err = ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
	assert_errno_throw("cannot detach: %s", strerror(errno));
	remove_cleanup(&pt_clup_s);
	/* wait for the process */
	if (!wait)
		return;

	/* when we wait child, ignore the SIGINT signal */
	signal(SIGINT, SIG_IGN);
	child_pid = -1;

	siginfo_t si;
	err = waitid(P_PID, old_pid, &si, WEXITED);
	if (err == -1) {
		ERROR(PTRACE, "wait error: %s\n", strerror(errno));
	} else {
		TRACE(PTRACE, "target process finished\n");
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
	assert_errno_throw("error in peeking regs: %s", strerror(errno));
	return ret;
}

void
ptrace_pokeuser(struct user_regs_struct s)
{
	assert(child_pid != -1);
	errno = 0;
	ptrace(PTRACE_SETREGS, child_pid, NULL, &s);
	assert_errno_throw("error in pokeing regs: %s",
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
	assert_throw(current_bkpt == 0,
			"a breakpoint has already been inserted");
	ptrace_dupmem(saved_bkpt_instr, address, INTINSTR_LEN);
	ptrace_updmem(bkpt_instr, address, INTINSTR_LEN);
	current_bkpt = address;
}

void
ptrace_cont(void)
{
	ptrace(PTRACE_CONT, child_pid, NULL,NULL);
	wait_and_check();
	return ;
}

void
ptrace_goto(uintptr_t addr)
{
	ptrace(PTRACE_POKEUSER, child_pid,
			(void*)offsetof(struct user_regs_struct, eip),
			addr);
	assert_errno_throw("error in setting eip: %s",
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
ptrace_push(void * data, int len)
{
	uint32_t esp = ptrace(PTRACE_PEEKUSER, child_pid,
			(void*)offsetof(struct user_regs_struct, esp), NULL);
	assert_errno_throw("cannot get esp");

	int err;
	uint32_t real_len = (len + 3) & 0xfffffffcUL;
	esp -= real_len;
	ptrace_updmem(data, esp, len);
	TRACE(PTRACE, "updmemory from 0x%x, len=%d\n", esp, len);

	err = ptrace(PTRACE_POKEUSER, child_pid,
			(void*)offsetof(struct user_regs_struct, esp), esp);
	return esp;
}


uint32_t ptrace_syscall(int no, int nr, ...)
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
				THROW(EXCEPTION_FATAL, "syscall args number too large");
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
	assert_errno_throw("ptrace syscall failed: %s",
			strerror(errno));
	
	wait_and_check();

	ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	assert_errno_throw("ptrace syscall failed: %s",
			strerror(errno));
	wait_and_check();

	/* restore the int80 command */
	ptrace_updmem(saved_syscall_instr, saved_regs.eip, SYSCALL_INSTR_LEN);

	TRACE(PTRACE, "retrive retval\n");
	/* retrive retval */
	uint32_t retval = ptrace(PTRACE_PEEKUSER, child_pid,
			offsetof(struct user_regs_struct, eax), NULL);

	/* restore register */
	TRACE(PTRACE, "restore regs\n");
	ptrace_pokeuser(saved_regs);
	return retval;
}

// vim:tabstop=4:shiftwidth=4

