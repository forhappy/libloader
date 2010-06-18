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
arch_restore_registers(pid_t pid, struct pusha_regs * regs,
		void * eip)
{
	TRACE(REPLAYER_HOST, "arch_restore_registers: new eip is %p\n", eip);
	struct user_regs_struct urs;
	int err;

	err = ptrace(PTRACE_GETREGS, pid, 0, &urs);
	if (err != 0)
		THROW_FATAL(EXP_PTRACE,
				"PTRACE_GETREGS failed: returns %d", err);

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

	err = ptrace(PTRACE_SETREGS, pid, 0, &urs);
	if (err != 0)
		THROW_FATAL(EXP_PTRACE, "PTRACE_SETREGS failes: returns %d", err);
}

// vim:ts=4:sw=4

