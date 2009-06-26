
#include "syscalls.h"
int SCOPE
post_close(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_close);
	write_eax(regs);
	return 0;
}

