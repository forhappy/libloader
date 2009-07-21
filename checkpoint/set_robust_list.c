

#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_set_robust_list(const struct syscall_regs * regs)
{
	
	write_eax(regs);
	state_vector.robust_list = regs->ebx;
	return 0;
}

#else

void
output_set_robust_list(void)
{
	printf("set_robust_list:\t%d\n", read_eax());
}
#endif

