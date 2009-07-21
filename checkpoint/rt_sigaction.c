
#include "syscalls.h"
#ifndef SYSCALL_PRINTER
int SCOPE
post_rt_sigaction(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax == 0) {
		void * d = &state_vector.sigactions[regs->ebx];
		uintptr_t act = regs->ecx;
		uintptr_t oact = regs->edx;
		int sigsetsize = regs->esi;

		write_obj(sigsetsize);
		if (sigsetsize != sizeof(k_sigset_t)) {
			INJ_WARNING("esi (%d) != %d\n",
					sigsetsize, sizeof(k_sigset_t));
			return 0;
		}

		write_obj(oact);
		if (act != 0)
			__dup_mem(d, act, sizeof(struct k_sigaction));

		if (oact != 0) {
			if (act == 0)
				__dup_mem(d, oact, sizeof(struct k_sigaction));
			write_mem(oact, sizeof(struct k_sigaction));
		}
	}
	return 0;
}

#else

void
output_rt_sigaction(void)
{
	int32_t ret = read_eax();
	if (ret == 0) {
		int sigsetsize;
		read_obj(sigsetsize);
		if (sigsetsize == sizeof(k_sigset_t)) {
			uintptr_t oact;
			read_obj(oact);
			if (oact != 0)
				skip(sizeof(struct k_sigaction));
		}
	}
	printf("rt_sigaction:\t%d\n", ret);
}
#endif

