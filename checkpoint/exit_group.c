
#include "syscalls.h"
#ifdef IN_INJECTOR
# include "injector.h"
#endif

#ifndef SYSCALL_PRINTER

int SCOPE
pre_exit_group(const struct syscall_regs * regs)
{
	/* save ebx */
	write_obj(regs->ebx);
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
#ifdef IN_INJECTOR
	/* needn't check signal */
	INJ_VERBOSE("target call exit_group(%d), exit normally\n", regs->ebx);
	INTERNAL_SYSCALL(exit_group, 1, regs->ebx);
#endif
	return 0;
}

#else

void
output_exit_group(int nr)
{
	int32_t code = read_int32();
	printf("exit_group(%d).\n", code);
}
#endif

