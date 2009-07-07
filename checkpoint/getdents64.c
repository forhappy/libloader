
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_getdents64(struct syscall_regs * regs)
{
	
	write_eax(regs);
	write_mem(regs->ecx, regs->eax);
	return 0;
}

#else

void
output_getdents64(void)
{
	
}
#endif

