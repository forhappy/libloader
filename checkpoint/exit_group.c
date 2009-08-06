
#include "syscalls.h"
#ifdef IN_INJECTOR
# include "injector.h"
#endif

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

int SCOPE
replay_exit_group(const struct syscall_regs * regs)
{
	/* needn't check signal */
#ifdef IN_INJECTOR
	INTERNAL_SYSCALL(exit_group, 1, regs->ebx);
#endif
	return 0;
}

#else

void
output_exit_group(void)
{
	printf("exit_group.\n");
}
#endif

