
#include "syscalls.h"
int SCOPE
post_rt_sigaction(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_rt_sigaction);
	write_eax(regs);
	if (regs->eax == 0) {
		void * d = &state_vector.sigactions[regs->ebx];
		uintptr_t s = regs->ecx;
		__dup_mem(d, s, sizeof(struct k_sigaction));
		if (regs->edx != 0)
			write_mem(regs->ebx, sizeof(struct k_sigaction));
	}
	return 0;
}

