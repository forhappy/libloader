/* 
 * rebranch_tpd.c
 * by WN @ Oct. 28, 2010
 * modified by HP.Fu @ Apr 30, 2011
 */

#include <loader/tls.h>
#include <loader/snitchaser_tpd.h>
#include <syscall.h>

#include <debug.h>

struct thread_private_data *
setup_self_tpd(void)
{
	struct tls_desc * td = get_self_tls_desc();
	struct thread_private_data * tpd = get_tpd(td);
	TRACE(TLS, "thread %d:%d tls start: %p, td: %p, tpd: %p\n",
			sys_getpid(), sys_gettid(), td->start_addr, td, tpd);
	tpd->tpd = tpd;
	struct thread_private_data * s_tpd = get_self_tpd();

	TRACE(TLS, "thread %d:%d self_tpd: %p\n",
			sys_getpid(), sys_gettid(), s_tpd);

	s_tpd->stack_top = s_tpd;
	TRACE(TLS, "stack_top is set to %p\n", s_tpd->stack_top);

	xmutex_init(&tpd->thread_mutex);

	return s_tpd;
}

void
free_aux_mem(struct thread_private_data * tpd)
{
	clean_mm_obj(&tpd->mm_stub);
}

// vim:ts=4:sw=4

