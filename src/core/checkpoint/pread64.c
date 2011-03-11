
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_pread64(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax > 0)
		write_mem(regs->ecx, regs->eax);
	return 0;
}

int SCOPE
replay_pread64(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();	
	if (eax > 0)
		read_mem(regs->ecx, eax);
	return eax;
}


#else

void
output_pread64(int nr)
{
	int32_t ret = read_eax();
	if (ret > 0)
		skip(ret);
	printf("pread64:\t%d\n", ret);
}
#endif

