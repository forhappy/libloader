
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_access(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_access(const struct syscall_regs * regs)
{
	int32_t eax;
	read_obj(eax);
	return eax;
}


#else

void
output_access(void)
{
	printf("access:\t%d\n", read_eax());
}
#endif

