
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
/* 
 * in the loader, those 2 vars are defined in snitchaser.c;
 * in the injector, those 2 vars are defined in wrapper.c;
 */

#define SIG_DFL	((void*)(0))	/* default signal handling */
#define SIG_IGN	((void*)(1))	/* ignore signal */
#define SIG_ERR	((void*)(-1))	/* error return from signal */

#define SA_NOCLDSTOP	0x00000001u
#define SA_NOCLDWAIT	0x00000002u
#define SA_SIGINFO	0x00000004u
#define SA_ONSTACK	0x08000000u
#define SA_RESTART	0x10000000u
#define SA_NODEFER	0x40000000u
#define SA_RESETHAND	0x80000000u

#define SA_NOMASK	SA_NODEFER
#define SA_ONESHOT	SA_RESETHAND

#define SA_RESTORER	0x04000000
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
}

