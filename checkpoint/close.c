
#include "syscalls.h"

#ifndef SYSCALL_PRINTER
int SCOPE
post_close(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_close(const struct syscall_regs * regs)
{
	/* open is real, the close must be real... */
	int32_t eax = read_int32();
	INJ_FATAL("in close\n");
#ifdef IN_INJECTOR
	INTERNAL_SYSCALL(close, 1, regs->ebx);
#endif
	INJ_FATAL("out close\n");
	return eax;
}


#else

void
output_close(void)
{
	printf("close:\t%d\n", read_eax());
}
#endif

