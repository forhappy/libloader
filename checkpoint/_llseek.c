
#include "syscalls.h"

typedef long long	__kernel_loff_t;
typedef __kernel_loff_t		loff_t;

#ifndef SYSCALL_PRINTER

int SCOPE
post__llseek(const struct syscall_regs * regs)
{
	write_eax(regs);
	uint32_t presult = regs->esi;
	write_obj(presult);
	if (regs->eax >= 0) {
		if (presult != 0)
			write_mem(presult, sizeof(loff_t));
	}
	return 0;
}

#else

void
output__llseek(void)
{
	int retval = read_eax();
	uint32_t presult;
	read_obj(presult);
	printf("_llseek:\t0x%x\n", retval);
	if (retval >= 0) {
		if (presult != 0)
			skip(sizeof(loff_t));
	}
}
#endif

