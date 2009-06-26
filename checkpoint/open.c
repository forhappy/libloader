

#include "syscalls.h"
int SCOPE
post_open(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_open);
	write_eax(regs);
	return 0;
}

