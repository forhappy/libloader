
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_utime(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_utime(const struct syscall_regs * regs)
{
	return read_int32();
}


#else

void
output_utime(void)
{
	printf("utime:\t0x%x\n", read_eax());
}
#endif

