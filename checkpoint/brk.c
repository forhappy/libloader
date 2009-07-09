
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_brk(struct syscall_regs * regs)
{
	write_eax(regs);
	state_vector.brk = regs->eax;
	return 0;
}

#else

void
output_brk(void)
{
	printf("brk:\t0x%x\n", read_eax());
}
#endif

