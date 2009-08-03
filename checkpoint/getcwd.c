
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_getcwd(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax > 0)
		write_mem(regs->ebx, regs->eax);
	return 0;
}

int SCOPE
replay_getcwd(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax > 0)
		read_mem(regs->ebx, eax);
	return eax;
}
#else

void
output_getcwd(void)
{
	int32_t eax = read_eax();
	printf("getcwd:\t0x%x\n", eax);
	if (eax > 0)
		skip(eax);
}
#endif

