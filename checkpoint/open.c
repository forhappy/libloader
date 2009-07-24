

#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_open(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_open(const struct syscall_regs * regs)
{
	/* we need open files for mmap... */
	int32_t eax = read_int32();
#ifdef IN_INJECTOR
	if (eax >= 0) {
		int32_t ret;
		ret = INTERNAL_SYSCALL(open, 3, regs->ebx, regs->ecx, regs->edx);
		ASSERT(eax == ret, "");
	}
#endif
	return eax;
}



#else

void
output_open(void)
{
	printf("open:\t%d\n", read_eax());
}
#endif

