
#include "syscalls.h"
#ifndef SYSCALL_PRINTER
int SCOPE
post_fcntl64(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

#else

void
output_fcntl64(void)
{
	printf("fcntl64:\t%d\n", read_eax());
}
#endif

