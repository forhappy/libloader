
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
pre_exit_group(const struct syscall_regs * regs)
{
	/* placeholder */
	return 0;
}

int SCOPE
post_exit_group(const struct syscall_regs * regs)
{
	/* placeholder */
	return 0;
}

#else

void
output_exit_group(void)
{
	printf("exit_group.\n");
}
#endif

