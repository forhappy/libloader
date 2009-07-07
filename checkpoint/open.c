

#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_open(struct syscall_regs * regs)
{
	
	write_eax(regs);
	return 0;
}

#else

void
output_open(void)
{
	
}
#endif

