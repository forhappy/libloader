
#include "syscalls.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_write(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

#else

void
output_write(void)
{
	int32_t ret = read_eax();
	printf("write:\t%d\n", ret);
}
#endif

