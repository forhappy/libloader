

#include "syscalls.h"
int SCOPE
post_set_robust_list(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_set_robust_list);
	write_eax(regs);
	state_vector.robust_list = regs->ebx;
	return 0;
}


