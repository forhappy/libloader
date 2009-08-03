
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_getpid(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_getpid(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	return eax;
}
#else

void
output_getpid(void)
{
	printf("getpid:\t0x%x\n", read_eax());
}
#endif

