
#include "syscalls.h"
int SCOPE
post_getdents64(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_getdents64);
	write_eax(regs);
	write_mem(regs->ecx, regs->eax);
	return 0;
}

