
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_unlink(struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

#else

void
output_unlink(void)
{
	printf("unlink:\t0x%x\n", read_eax());
}
#endif

