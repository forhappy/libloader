
#include "syscalls.h"
int SCOPE
post_sigprocmask(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_sigprocmask);
	write_eax(regs);
	return 0;
}

