
#include "syscalls.h"

#ifndef SYSCALL_PRINTER
int SCOPE
post_close(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_close(const struct syscall_regs * regs)
{
	return read_int32();
}


#else

void
output_close(void)
{
	printf("close:\t%d\n", read_eax());
}
#endif

