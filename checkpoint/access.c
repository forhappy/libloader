
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_access(const const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

#else

void
output_access(void)
{
	printf("access:\t%d\n", read_eax());
}
#endif

