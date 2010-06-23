/* 
 * arch_replayer.c
 * by WN @ Jun. 18, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <host/exception.h>
#include <host/arch_replayer_helper.h>

#include <sys/user.h>
#include <sys/ptrace.h>

void
ptrace_get_regset(pid_t pid, struct user_regs_struct * purs)
{
	errno = 0;
	int err = ptrace(PTRACE_GETREGS, pid, 0, purs);
	ETHROW_FATAL(EXP_PTRACE, "PTRACE_GETREGS failed: returns %d", err);
}

void
ptrace_set_regset(pid_t pid, struct user_regs_struct * purs)
{
	errno = 0;
	int err = ptrace(PTRACE_SETREGS, pid, 0, purs);
	ETHROW_FATAL(EXP_PTRACE, "PTRACE_SETREGS failed: returns %d", err);
}

uintptr_t
ptrace_get_eip(pid_t pid)
{
	errno = 0;
	uintptr_t eip = ptrace(PTRACE_PEEKUSER, pid,
			(void*)(offsetof(struct user_regs_struct, eip)),
			NULL);
	ETHROW_FATAL(EXP_PTRACE, "PTRACE_PEEKUSER failed");
	return eip;
}

void
ptrace_set_eip(pid_t pid, uintptr_t eip)
{
	errno = 0 ;
	int err = ptrace(PTRACE_POKEUSER, pid,
			(void*)(offsetof(struct user_regs_struct, eip)),
			eip);
	ETHROW_FATAL(EXP_PTRACE, "PTRACE_PEEKUSER failes: returns %d", err);
}

void
arch_restore_registers(pid_t pid, struct pusha_regs * regs,
		void * eip)
{
	TRACE(REPLAYER_HOST, "arch_restore_registers: new eip is %p\n", eip);
	struct user_regs_struct urs;

	ptrace_get_regset(pid, &urs);

#define set_reg(x)	urs.x = regs->x

	set_reg(eax);
	set_reg(ebx);
	set_reg(ecx);
	set_reg(edx);
	set_reg(esi);
	set_reg(edi);
	set_reg(esp);
	set_reg(ebp);

#undef set_reg

	if (eip != NULL)
		urs.eip = (uintptr_t)eip;

	ptrace_set_regset(pid, &urs);
}

// vim:ts=4:sw=4

