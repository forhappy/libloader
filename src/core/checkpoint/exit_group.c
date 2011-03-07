
#include "syscalls.h"
#ifdef IN_INJECTOR
# include "../libwrapper/injector.h"
#endif

extern void SCOPE ATTR(noreturn)
replay_sighandler(int signum, const struct syscall_regs * regs);
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
	int32_t retval = read_int32();
	ASSERT(retval == regs->ebx, regs, "exit code inconsistent\n");
	
	/* whether there is signal? */
	int16_t f;
	int err;
	err = INTERNAL_SYSCALL(read, 3, logger_fd, &f, sizeof(f));
	if (err <= 0) {
		INJ_VERBOSE("target call exit_group(%d), exit normally\n", regs->ebx);
		INTERNAL_SYSCALL(exit_group, 1, regs->ebx);
	} else {
		INJ_FORCE("signal %d raise before exit_group\n", -f - 1);
		replay_sighandler(-f - 1, regs);
	}
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

