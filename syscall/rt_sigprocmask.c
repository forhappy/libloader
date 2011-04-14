
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
#include "signal_defs.h"


int SCOPE
post_rt_sigprocmask(const struct syscall_regs * regs)
{
	write_eax(regs);
#if 0
	if (regs->eax == 0) {
		int how = regs->ebx;
		int set = regs->ecx;
		int oset = regs->edx;
		int sigsetsize = regs->esi;
		write_obj(sigsetsize);
		if (sigsetsize != sizeof(k_sigset_t)) {
			INJ_WARNING("sigsetsize %d != %d\n",
					sigsetsize, sizeof(k_sigset_t));
			return 0;
		}

		if (set) {
			k_sigset_t mask;
			__dup_mem(&mask, set, sigsetsize);

			sigdelsetmask(&mask, sigmask(SIGKILL)|sigmask(SIGSTOP));
			if (how == SIG_BLOCK) {
				sigorsets(&state_vector.sigmask,
					&state_vector.sigmask,
					&mask);
			} else if (how == SIG_UNBLOCK) {
				signandsets(&state_vector.sigmask,
					&state_vector.sigmask,
					&mask);
			} else {
				/*  SIG_SETMASK */
				state_vector.sigmask = mask;
			}
		}
		
		write_obj(oset);
		if (oset) {
			if (set == 0) {
				k_sigset_t mask;
				__dup_mem(&mask, oset, sigsetsize);
				state_vector.sigmask = mask;
			}
			write_mem(oset, sigsetsize);
		}
	}
	return 0;
#endif
}

