
#include "syscalls.h"
int SCOPE
post_brk(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_brk);
	write_eax(regs);
	state_vector.brk = regs->eax;
	return 0;
}

