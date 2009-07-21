
#include "syscalls.h"


struct k_timespec {
        long       ts_sec;
        long       ts_nsec;
};


#ifndef SYSCALL_PRINTER

int SCOPE
post_nanosleep(const struct syscall_regs * regs)
{
	write_eax(regs);
	uintptr_t o = regs->ecx;
	write_obj(o);
	if (o != 0)
		write_mem(o, sizeof(struct k_timespec));
	return 0;
}

#else

void
output_nanosleep(void)
{
	int32_t res;
	res = read_eax();

	uintptr_t o;
	read_obj(o);

	if (o != 0)
		skip(sizeof(struct k_timespec));
	printf("nanosleep:\t%d\n", res);
}
#endif

