
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_sigprocmask(struct syscall_regs * regs)
{
	
	write_eax(regs);
	return 0;
}

#else

void
output_sigprocmask(void)
{
	
}
#endif

