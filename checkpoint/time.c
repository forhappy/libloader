

#include "syscalls.h"

#ifndef SYSCALL_PRINTER

typedef long		__kernel_time_t;
typedef __kernel_time_t		time_t;


int SCOPE
post_time(struct syscall_regs * regs)
{
	write_eax(regs);
	write_obj(regs->ebx);
	if (regs->ebx != 0)
		write_mem(regs->ebx, sizeof(time_t));
	return 0;
}

#else

void
output_time(void)
{
	printf("time:\t%d\n", read_eax());

	uint32_t ebx;
	read_obj(ebx);
	if (ebx != 0)
		skip(sizeof(time_t));
}
#endif

