
#include "syscalls.h"
#ifndef SYSCALL_PRINTER
int SCOPE
post_rt_sigaction(struct syscall_regs * regs)
{
	
	write_eax(regs);
	if (regs->eax == 0) {
		void * d = &state_vector.sigactions[regs->ebx];
		uintptr_t act = regs->ecx;
		uintptr_t oact = regs->edx;
		int sigsetsize = regs->esi;

		if (sigsetsize != sizeof(k_sigset_t)) {
			INJ_WARNING("esi (%d) != %d\n",
					sigsetsize, sizeof(k_sigset_t));
			return 0;
		}

		if (act != 0)
			__dup_mem(d, act, sizeof(struct k_sigaction));

		if (oact != 0) {
			if (act == 0)
				__dup_mem(d, oact, sizeof(struct k_sigaction));
			write_mem(regs->ebx, sizeof(struct k_sigaction));
		}
	}
	return 0;
}

#else

void
output_rt_sigaction(void)
{
	
}
#endif

