
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_set_tid_address(const struct syscall_regs * regs)
{
	write_eax(regs);
	state_vector.clear_child_tid = regs->ebx;
	return 0;
}

int SCOPE
replay_set_tid_address(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax >= 0) {
#ifdef IN_INJECTOR
		INTERNAL_SYSCALL(set_tid_address, 1,
				regs->ebx);
#endif
	}
	return eax;
}

#else

void
output_set_tid_address(int nr)
{
	printf("set_tid_address:\t%d\n", read_eax());
}
#endif

