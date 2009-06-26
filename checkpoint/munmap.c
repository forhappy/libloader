
#include "syscalls.h"
int SCOPE
post_munmap(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_munmap);
	write_eax(regs);
	return 0;
}

