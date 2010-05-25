/* 
 * log_syscalls.c
 * by WN @ May. 25, 2010
 */

#include <common/debug.h>
#include "log_syscalls.h"

static void
pre_log_syscall(struct pusha_regs * pregs)
{
	breakpoint();
	FATAL(LOG_SYSCALL, "come here!\n");
}


static void
post_log_syscall(struct pusha_regs * pregs)
{
	breakpoint();
	FATAL(LOG_SYSCALL, "come here!\n");
}

void
pre_log_syscall_int80(struct pusha_regs regs)
{
	pre_log_syscall(&regs);
}

void
post_log_syscall_int80(struct pusha_regs regs)
{
	post_log_syscall(&regs);
}

void
pre_log_syscall_vdso(struct pusha_regs regs)
{
	pre_log_syscall(&regs);
}

void
post_log_syscall_vdso(struct pusha_regs regs)
{
	post_log_syscall(&regs);
}

// vim:ts=4:sw=4

