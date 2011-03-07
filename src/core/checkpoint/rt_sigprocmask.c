
#include "syscalls.h"
#ifdef IN_INJECTOR
# include "../libwrapper/injector.h"
#endif

#include "../libwrapper/signal_defs.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_rt_sigprocmask(const struct syscall_regs * regs)
{
	write_eax(regs);
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
}

int SCOPE
replay_rt_sigprocmask(const struct syscall_regs * regs)
{
	int32_t ret = read_int32();
	if (ret == 0) {
		int sigsetsize = read_int32();
		ASSERT(sigsetsize == regs->esi, regs, "");
		if (sigsetsize == sizeof(k_sigset_t)) {
			int oset = read_int32();
			ASSERT(oset == regs->edx, regs, "");
			if (oset)
				read_mem(oset, sigsetsize);
		}
	}
	return ret;
}
#else

void
output_rt_sigprocmask(int nr)
{
	int32_t ret = read_eax();
	if (ret == 0) {
		int sigsetsize;
		read_obj(sigsetsize);
		if (sigsetsize == sizeof(k_sigset_t)) {
			int32_t oset;
			read_obj(oset);
			if (oset)
				skip(sigsetsize);
		}
	}
	printf("rt_sigprocmask:\t%d\n", ret);
}
#endif

