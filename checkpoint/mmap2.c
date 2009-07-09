
#include "syscalls.h"
#ifndef SYSCALL_PRINTER
int SCOPE
post_mmap2(struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

#else

void
output_mmap2(void)
{
	printf("mmap2:\t0x%x\n", read_eax());
}
#endif

