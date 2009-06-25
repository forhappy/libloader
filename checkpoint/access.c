
#include "syscalls.h"
int SCOPE
post_access(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_access);
	write_eax(regs);
	return 0;
}

