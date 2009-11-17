
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
	/* see dup2's comment, this is not a simple syscall */
	int32_t eax = read_int32();
	return eax;
}
#else

void
output_dup(int nr)
{
	printf("dup:\t%d\n", read_eax());
}
#endif

