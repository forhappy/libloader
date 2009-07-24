

#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_set_robust_list(const struct syscall_regs * regs)
{
	
	write_eax(regs);
	state_vector.robust_list = regs->ebx;
	return 0;
}

int SCOPE
replay_set_robust_list(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax >= 0) {
#ifdef IN_INJECTOR
		int32_t ret;
		ret = INTERNAL_SYSCALL(set_robust_list, 2,
				regs->ebx, regs->ecx);
		ASSERT(ret == eax, "!@!@#\n");
#endif
	}
	return eax;
}


#else

void
output_set_robust_list(void)
{
	printf("set_robust_list:\t%d\n", read_eax());
}
#endif

