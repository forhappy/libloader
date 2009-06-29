
#include "syscalls.h"
int SCOPE
post_set_tid_address(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_set_tid_address);
	write_eax(regs);
	state_vector.clear_child_tid = regs->ebx;
	return 0;
}

