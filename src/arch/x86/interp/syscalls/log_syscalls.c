/* 
 * log_syscalls.c
 * by WN @ May. 25, 2010
 */

#include <common/debug.h>
#include "log_syscalls.h"

static int
pre_log_syscall(struct pusha_regs * pregs)
{
	VERBOSE(LOG_SYSCALL, "come here, eax=%d, ebx=%d\n",
			pregs->eax, pregs->ebx);
	return 0;
}


static void
post_log_syscall(struct pusha_regs * pregs)
{
	VERBOSE(LOG_SYSCALL, "come here, eax=0x%x\n",
			pregs->eax);
}

int
pre_log_syscall_int80(struct pusha_regs regs)
{
	return pre_log_syscall(&regs);
}

void
post_log_syscall_int80(struct pusha_regs regs)
{
	post_log_syscall(&regs);
}

int
pre_log_syscall_vdso(struct pusha_regs regs)
{
	return pre_log_syscall(&regs);
}

void
post_log_syscall_vdso(struct pusha_regs regs)
{
	post_log_syscall(&regs);
}

// vim:ts=4:sw=4

