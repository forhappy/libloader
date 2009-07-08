
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_access(struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

#else

void
output_access(void)
{
	int32_t ret;
	read_eax(ret);
	printf("access:\t%d\n", ret);
}
#endif

