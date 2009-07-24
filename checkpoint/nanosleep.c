
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

int SCOPE
replay_nanosleep(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	uintptr_t o = regs->ecx;
	read_obj(o);
	if (o != 0)
		read_mem(o, sizeof(struct k_timespec));
	return eax;
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

