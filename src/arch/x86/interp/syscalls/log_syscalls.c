/* 
 * log_syscalls.c
 * by WN @ May. 25, 2010
 */

#include <config.h>
#include <common/debug.h>
#include <interp/logger.h>
#include <xasm/tls.h>

#include "log_syscalls.h"
#include "syscall_table.h"

static int
pre_log_syscall(struct pusha_regs * regs)
{
	VERBOSE(LOG_SYSCALL, "come here, eax=%d, ebx=%d\n",
			regs->eax, regs->ebx);

	struct thread_private_data * tpd = get_tpd();

	int nr = regs->eax;
	tpd->current_syscall_nr = nr;

	append_buffer(&nr, sizeof(nr));
	if (syscall_table[nr].pre_handler) {
		return syscall_table[nr].pre_handler(regs);
	} else {
		FATAL(LOG_SYSCALL, "doesn't support syscall %d\n", nr);
	}
}


static void
post_log_syscall(struct pusha_regs * regs)
{

	struct thread_private_data * tpd = get_tpd();
	int nr = tpd->current_syscall_nr;

	VERBOSE(LOG_SYSCALL, "come here, eax=0x%x\n",
			regs->eax);
	if (syscall_table[nr].post_handler) {
		syscall_table[nr].post_handler(regs);
		return;
	} else {
		FATAL(LOG_SYSCALL, "doesn't support syscall %d\n", nr);
	}
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

