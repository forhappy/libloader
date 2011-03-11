
#include "syscalls.h"

typedef long		__kernel_suseconds_t;
typedef long		__kernel_time_t;
typedef __kernel_suseconds_t	suseconds_t;
typedef __kernel_time_t		time_t;

struct k_timeval {
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* microseconds */
};

struct itimerval {
	struct k_timeval it_interval;	/* timer interval */
	struct k_timeval it_value;	/* current value */
};


#ifndef SYSCALL_PRINTER

int SCOPE
post_setitimer(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		write_obj(regs->edx);
		if (regs->edx != 0)
			write_mem(regs->edx, sizeof(struct itimerval));
	}
	return 0;
}

int SCOPE
replay_setitimer(const struct syscall_regs * regs)
{
	int eax = read_int32();
	if (eax >= 0) {
		uint32_t ovalue = read_uint32();
		ASSERT(ovalue == regs->edx, regs, "ovalue inconsistent\n");
		if (ovalue != 0)
			read_mem(ovalue, sizeof(struct itimerval));
	}
	return eax;
}


#else

void
output_setitimer(int nr)
{
	int32_t ret;
	ret = read_eax();
	printf("setitimer:\t%d\n", ret);
	if (ret >= 0) {
		uint32_t ovalue = read_uint32();
		if (ovalue != 0)
			skip(sizeof(struct itimerval));
	}
	
}
#endif

