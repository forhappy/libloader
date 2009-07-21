
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_mprotect(const struct syscall_regs * regs)
{
	
	write_eax(regs);
	return 0;
}

#else

void
output_mprotect(void)
{
	printf("mprotect:\t%d\n", read_eax());
}
#endif

