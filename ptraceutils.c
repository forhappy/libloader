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
	err = waitid(P_PID, child_pid, &si, WEXITED | WSTOPPED);
	assert_throw(err >= 0, "wait failed");

	/* Check for siginfo */
	if (si.si_code != CLD_TRAPPED) {
		int old_pid = child_pid;
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);
		child_pid = -1;
		FORCE(SYSTEM, "signal: %d\n", si.si_status);
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
	assert(addr % 4 == 0);
	assert(len % 4 == 0);

	int i;
	for (i = 0; i < len; i += 4) {
		long val;
		val = ptrace(PTRACE_PEEKDATA, child_pid, addr + i, NULL);
		assert_errno_throw("ptrace peek data failed: %s", 
				strerror(errno));
		*((uint32_t*)(dst + i)) = val;
	}
	return;
}

void
ptrace_updmem(void * src, uintptr_t addr, int len)
{
#if 0
	ptrace(PTRACE_POKEDATA, child_pid, addr, *(uint32_t*)(src));
	return;
#endif

	assert(child_pid != -1);

	uint32_t * start, * end, * p;
	start = (addr % 4 == 0) ? src : src + (4 - (addr % 4));
	end = src + len - (addr + len) % 4;

	p = start;

	while (p < end) {
		ptrace(PTRACE_POKEDATA, child_pid,
				((void*)(addr) + ((void*)p - src)),
				(void*)(*p));
		assert_errno_throw("ptrace peek data failed: %s",
				strerror(errno));
		p ++;
	}

	/* the start and end fragment */
	if (addr % 4 != 0) {
		/* peek the start word */
		uint32_t word = ptrace(PTRACE_PEEKDATA, child_pid, (void*)(addr & 0xfffffffcUL), NULL);
		assert_errno_throw("ptrace peek data failed: %s", strerror(errno));
		for (int i = addr % 4; i < 4; i++) {
			uint8_t * ptr = (uint8_t*)(&word);
			if (i - addr % 4 > len)
				break;
			ptr[i] = ((uint8_t*)(src))[i - addr % 4];
		}
		/* poke the word back*/
		ptrace(PTRACE_POKEDATA, child_pid,
				(void*)(addr & 0xfffffffcUL), (void*)word);
		assert_errno_throw("ptrace poke data failed: %s", strerror(errno));
	}

	/* end fragment */
	if ((addr + len) % 4 != 0) {
		/* check a special situation */
		if (((addr + len) & 0xfffffffcUL) > addr) {
			/* peek the last word */
			uint32_t word = ptrace(PTRACE_PEEKDATA, child_pid,
					(void*)((addr + len) & 0xfffffffcUL), NULL);
			assert_errno_throw("ptrace peek data failed: %s", strerror(errno));
			int i, j;
			for (i = (addr + len) % 4 - 1, j = 1; i >= 0; i--, j++) {
				uint8_t * ptr = (uint8_t*)&word;
				ptr[i] = ((uint8_t*)(src))[len - j];
			}

			/* poke the word back*/
			ptrace(PTRACE_POKEDATA, child_pid,
					(void*)((addr + len) & 0xfffffffcUL), (void*)word);
			assert_errno_throw("ptrace poke data failed: %s", strerror(errno));
		}
	}
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
#define peek_reg(x)	ret.x = ptrace(PTRACE_PEEKUSER, child_pid, \
		(void*)offsetof(struct user_regs_struct, x), NULL)
	peek_reg(orig_eax);
	peek_reg(eax);
	peek_reg(ebx);
	peek_reg(ecx);
	peek_reg(edx);
	peek_reg(esp);
	peek_reg(eip);
	peek_reg(ebp);
	assert_errno_throw("error in peeking regs: %s", strerror(errno));
#undef peek_reg
	return ret;
}

void
ptrace_pokeuser(struct user_regs_struct s)
{
	assert(child_pid != -1);
	errno = 0;
#define poke_reg(x)	ptrace(PTRACE_POKEUSER, child_pid, \
		(void*)offsetof(struct user_regs_struct, x), s.x)
	poke_reg(orig_eax);
	poke_reg(eax);
	poke_reg(ebx);
	poke_reg(ecx);
	poke_reg(edx);
	poke_reg(esp);
	poke_reg(eip);
	poke_reg(ebp);
	assert_errno_throw("error in pokeing regs: %s\n",
			strerror(errno));
#undef poke_reg
}

#define ARCH_INTINSTR	{'\x33'}
static const uint8_t int_instr[] = ARCH_INTINSTR;
static uintptr_t current_bkpt = 0;
#define BKPT_SAVE_SIZE	(((sizeof(int_instr) + 2) / 4 + 1) * 4)
static uint8_t saved_instr[BKPT_SAVE_SIZE];

void
ptrace_insert_bkpt(uintptr_t address)
{

	assert_throw(current_bkpt == 0,
			"a breakpoint has already been inserted");

	/* compute the start peek address and offset of the instr */
	uintptr_t addr_start, addr_end, offset;
	addr_start = address & 0xfffffffcUL;
	addr_end =((address + sizeof(int_instr)) + 3) & 0xfffffffcUL;

	int duplen = addr_end - addr_start;
	assert(duplen <= BKPT_SAVE_SIZE);
	offset = addr_start - address;
	ptrace_dupmem(saved_instr, addr_start, duplen);

	uint8_t tmp[BKPT_SAVE_SIZE];
	memcpy(tmp, saved_instr, BKPT_SAVE_SIZE);
	memcpy(tmp + offset, int_instr, sizeof(int_instr));

	ptrace_updmem(tmp, addr_start, duplen);
}

// vim:tabstop=4:shiftwidth=4

