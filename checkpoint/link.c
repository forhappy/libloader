
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_link(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_link(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	return eax;
}
#else

void
output_link(int nr)
{
	printf("link:\t0x%x\n", read_eax());
}
#endif

