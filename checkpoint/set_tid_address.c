
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_set_tid_address(const struct syscall_regs * regs)
{
	
	write_eax(regs);
	state_vector.clear_child_tid = regs->ebx;
	return 0;
}

#else

void
output_set_tid_address(void)
{
	printf("set_tid_address:\t%d\n", read_eax());
}
#endif

