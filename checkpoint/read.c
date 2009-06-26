
#include "syscalls.h"

int SCOPE
post_read(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_read);
	write_eax(regs);
	if (regs->eax > 0)
		write_mem(regs->ecx, regs->eax);
	return 0;
}

