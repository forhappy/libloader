
#include "syscalls.h"
int SCOPE
post_mprotect(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_mprotect);
	write_eax(regs);
	return 0;
}

