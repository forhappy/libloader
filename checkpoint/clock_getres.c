
#include "syscalls.h"

typedef int		__kernel_clockid_t;
typedef __kernel_clockid_t	clockid_t;
struct k_timespec {
        long       ts_sec;
        long       ts_nsec;
};

#ifndef SYSCALL_PRINTER

int SCOPE
post_clock_getres(struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		uint32_t tp = regs->ecx;
		write_mem(tp, sizeof(struct k_timespec));
	}
	return 0;
}

#else

void
output_clock_getres(void)
{
	int ret = read_eax();
	printf("clock_getres:\t0x%x\n", ret);
	if (ret >= 0)
		skip(sizeof(struct k_timespec));
}
#endif

