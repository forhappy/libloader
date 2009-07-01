
#include "syscalls.h"
int SCOPE
post_fcntl64(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_fcntl64);
	write_eax(regs);
	return 0;
}

