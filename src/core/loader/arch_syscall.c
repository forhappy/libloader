/* 
 * arch_syscall.c
 * by WN @ Nov. 28, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <interp/syscall_tpd.h>
#include <interp/rebranch_tpd.h>
#include <interp/arch_syscall.S>
#include <interp/logger.h>
#include <interp/marks.h>
#include <interp/codecache.h>

#include <interp/syscalls/syscall_table.h>

int
arch_pre_syscall(struct pusha_regs * r)
{
	DEBUG(LOG_SYSCALL, "pre pocessing of sysall %d\n", r->eax);
	struct syscall_tpd_stub * s = get_self_syscall_ptr();
	struct thread_private_data * tpd = get_self_tpd();
	struct basic_block * curr_bb = tpd->cc_stub.curr_bb;
	assert(curr_bb != NULL);

	s->curr_syscall_nr = r->eax;
	s->bkup_regs = *r;
	s->bkup_stack_top = tpd->old_stack_top;

	s->is_posted = FALSE;

	/* record syscall head */
	append_logger_ptr(curr_bb->branch);
	append_logger_u32(SYSCALL_MARK);

	r->esp = (uintptr_t)tpd->old_stack_top;
	append_logger(r, sizeof(*r));
	r->esp = s->bkup_regs.esp;

	assert(r->eax < SYSCALL_TABLE_SZ);

	if (syscall_table[r->eax].pre_handler != NULL) {
		TRACE(LOG_SYSCALL, "goto pre_handler %d(%p)\n",
				r->eax, syscall_table[r->eax].pre_handler);
		return syscall_table[r->eax].pre_handler(r);
	}

	FATAL(LOG_SYSCALL, "doesn't support syscall %d\n", r->eax);

	return -1;
}

int
arch_post_syscall(struct pusha_regs * r)
{
	struct thread_private_data * tpd = get_self_tpd();
	struct syscall_tpd_stub * s = &tpd->syscall_stub;
	int nr = s->curr_syscall_nr;
	DEBUG(LOG_SYSCALL, "post pocessing of sysall %d = %d (0x%x)\n",
			nr, r->eax, r->eax);

	if (s->is_posted) {
		DEBUG(LOG_SYSCALL, "post pocessing has been done\n");
		return 0;
	}

	append_logger_u32(NO_SIGNAL_MARK);
	append_logger_int(nr);

	uintptr_t esp = r->esp;
	r->esp = (uintptr_t)tpd->old_stack_top;
	append_logger(r, sizeof(*r));
	r->esp = esp;

	if (syscall_table[nr].post_handler != NULL) {
		TRACE(LOG_SYSCALL, "goto post_handler %d(%p)\n",
				nr, syscall_table[nr].post_handler);
		int retval = syscall_table[nr].post_handler(r);
		s->is_posted = TRUE;
		return retval;
	}
	FATAL(LOG_SYSCALL, "syscall %d not impl\n", nr);
	return -1;
}

// vim:ts=4:sw=4

