
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_dup(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_dup(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	return eax;
}
#else

void
output_dup(void)
{
	printf("dup:\t%d\n", read_eax());
}
#endif

