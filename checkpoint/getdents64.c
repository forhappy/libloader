
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_getdents64(struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		write_mem(regs->ecx, regs->eax);
	}
	return 0;
}

#else

void
output_getdents64(void)
{
	int32_t ret;
	read_eax(ret);
	if (ret >= 0)
		skip(ret);
	
}
#endif

