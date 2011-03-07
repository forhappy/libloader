
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_getdents64(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		write_mem(regs->ecx, regs->eax);
	}
	return 0;
}

int SCOPE
replay_getdents64(const struct syscall_regs * regs)
{
	int eax = read_int32();
	if (eax >= 0) {
		read_mem(regs->ecx, eax);
	}
	return eax;
}


#else

void
output_getdents64(int nr)
{
	int32_t ret;
	ret = read_eax();
	printf("getdents64:\t%d\n", ret);
	if (ret >= 0)
		skip(ret);
	
}
#endif

