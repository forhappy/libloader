
#include "syscalls.h"
int SCOPE
post_rt_sigaction(struct syscall_regs * regs)
{
//	INJ_FORCE("come to rt_sigaction\n");
	write_syscall_nr(__NR_rt_sigaction);
//	INJ_FORCE("write syscall nr\n");
	write_eax(regs);
	if (regs->eax == 0) {
		void * d = &state_vector.sigactions[regs->ebx];
		uintptr_t s = regs->ecx;
//		INJ_FORCE("dup mem from 0x%x to %p\n", s, d);
		__dup_mem(d, s, sizeof(struct k_sigaction));
		if (regs->edx != 0)
			write_mem(regs->ebx, sizeof(struct k_sigaction));
	}
	return 0;
}

