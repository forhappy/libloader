
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_pipe(const struct syscall_regs * regs)
{
	int32_t eax = regs->eax;
	write_eax(regs);
	if (eax >= 0)
		write_mem(regs->ebx, 2 * sizeof(unsigned long));
	return 0;
}

int SCOPE
replay_pipe(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax >= 0)
		read_mem(regs->ebx, 2 * sizeof(unsigned long));
	return eax;
}
#else

void
output_pipe(int nr)
{
	int32_t eax = read_int32();
	if (eax >= 0)
		skip(2 * sizeof(unsigned long));
	printf("pipe:\t%d\n", eax);
	return;
}
#endif

