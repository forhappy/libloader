
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_getppid(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_getppid(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	return eax;
}
#else

void
output_getppid(void)
{
	printf("getppid:\t%d\n", read_eax());
}
#endif

