
#include "syscalls.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_write(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_write(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
#ifdef IN_INJECTOR
	if ((regs->ebx == 1) || (regs->ebx == 2))
		INTERNAL_SYSCALL(write, 3,
				regs->ebx, regs->ecx, regs->edx);
#endif
	return eax;
}

#else

void
output_write(void)
{
	int32_t ret = read_eax();
	printf("write:\t%d\n", ret);
}
#endif

