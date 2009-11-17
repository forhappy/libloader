
#include "syscalls.h"

struct rlimit {
	unsigned long	rlim_cur;
	unsigned long	rlim_max;
};

/* ugetrlimit is actually getrlimit in kernel,
 * libc's getrlimit is old_getrlimit in kernel  */

#ifndef SYSCALL_PRINTER
int SCOPE
post_ugetrlimit(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax == 0) {
		write_mem(regs->ecx, sizeof(struct rlimit));
	}
	return 0;
}

int SCOPE
replay_ugetrlimit(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax == 0) {
		read_mem(regs->ecx, sizeof(struct rlimit));
	}
	return eax;
}

#else

void
output_ugetrlimit(int nr)
{
	int32_t ret = read_eax();
	if (ret == 0)
		skip(sizeof(struct rlimit));
	printf("ugetrlimit:\t%d\n", ret);
}
#endif

