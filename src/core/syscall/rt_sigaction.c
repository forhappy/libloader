
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
/* 
 * in the loader, those 2 vars are defined in snitchaser.c;
 * in the injector, those 2 vars are defined in wrapper.c;
 */

int SCOPE
post_rt_sigaction(const struct syscall_regs * regs)
{
	write_eax(regs);
#if 0
	int signo = regs->ebx;
	if ((signo == SIGKILL) || (signo == SIGSTOP))
		return 0;
	if ((signo < 1) || (signo > K_NSIG))
		return 0;

	/* rt_sigaction should not fail... */
	ASSERT(regs->eax >= 0, regs, "rt_sigaction failed, we cannot handle...\n");

	uintptr_t act = regs->ecx;
	uintptr_t oact = regs->edx;
	int sigsetsize = regs->esi;

	write_obj(sigsetsize);
	write_obj(oact);
	write_obj(act);

	/* we need copy modified act back */
	struct k_sigaction * s = &state_vector.sigactions[regs->ebx];
	if (act != 0)
		__upd_mem(act, s, sizeof(*s));

	if (sigsetsize != sizeof(k_sigset_t)) {
		INJ_WARNING("esi (%d) != %d\n",
				sigsetsize, sizeof(k_sigset_t));
		return 0;
	}

	if (oact != 0) {
		/* we modify the result */
		struct k_sigaction d;
		__dup_mem(&d, oact, sizeof(d));
		if (d.sa_handler == (void*)wrapped_sighandler) {
			struct k_sigaction * p = &(state_vector.sigactions[regs->ebx]);
			d = *p;
			__upd_mem(oact, &d, sizeof(d));
		}
		write_mem(oact, sizeof(struct k_sigaction));
	}
	return 0;
#endif
	return 0;
}

