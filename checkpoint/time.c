

#include "syscalls.h"

#ifndef SYSCALL_PRINTER

typedef long		__kernel_time_t;
typedef __kernel_time_t		time_t;


int SCOPE
post_time(const struct syscall_regs * regs)
{
	write_eax(regs);
	write_obj(regs->ebx);
	if (regs->ebx != 0)
		write_mem(regs->ebx, sizeof(time_t));
	return 0;
}

int SCOPE
replay_time(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	int32_t ebx = read_int32();
	ASSERT(ebx == regs->ebx, regs, "");
	if (ebx != 0)
		read_mem(ebx, sizeof(time_t));
	return eax;
}

#else

void
output_time(int nr)
{
	printf("time:\t%d\n", read_eax());

	uint32_t ebx;
	read_obj(ebx);
	if (ebx != 0)
		skip(sizeof(time_t));
}
#endif

