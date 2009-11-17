
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_munmap(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_munmap(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax >= 0) {
#ifdef IN_INJECTOR
		INTERNAL_SYSCALL(munmap, 2,
				regs->ebx, regs->ecx);
#endif
	}
	return eax;
}


#else

void
output_munmap(int nr)
{
	printf("munmap:\t%d\n", read_eax());
}
#endif

