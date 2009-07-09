
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_read(struct syscall_regs * regs)
{
	
	write_eax(regs);
	if (regs->eax > 0)
		write_mem(regs->ecx, regs->eax);
	return 0;
}

#else

void
output_read(void)
{
	int32_t ret = read_eax();
	if (ret > 0)
		skip(ret);
	printf("read:\t%d\n", ret);
}
#endif

