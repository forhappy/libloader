
#include "syscalls.h"

#ifndef SYSCALL_PRINTER
int SCOPE
post_close(struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

#else

void
output_close(void)
{
	int32_t ret;
	read_eax(ret);
	printf("close:\t%d\n", ret);
}
#endif

