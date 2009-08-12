
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_simple(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_simple(const struct syscall_regs * regs)
{
	return read_int32();
}


#else

void
output_simple(int nr)
{
	printf("%s:\t0x%x\n", syscall_table[nr].syscall_name, read_eax());
}
#endif

