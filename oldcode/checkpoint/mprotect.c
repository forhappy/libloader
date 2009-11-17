
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_mprotect(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}


int SCOPE
replay_mprotect(const struct syscall_regs * regs)
{
	int eax = read_int32();
	if (eax >= 0) {
#ifdef IN_INJECTOR
		INTERNAL_SYSCALL(mprotect, 3,
				regs->ebx, regs->ecx, regs->edx);
#endif
	}
	return eax;
}


#else

void
output_mprotect(int nr)
{
	printf("mprotect:\t%d\n", read_eax());
}
#endif

