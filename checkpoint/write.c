
#include "syscalls.h"
int SCOPE
post_write(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_write);
	write_eax(regs);
	return 0;
}

