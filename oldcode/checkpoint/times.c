
#include "syscalls.h"

struct tms {
	clock_t tms_utime;
	clock_t tms_stime;
	clock_t tms_cutime;
	clock_t tms_cstime;
};

#ifndef SYSCALL_PRINTER

int SCOPE
post_times(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		write_mem(regs->ebx, sizeof(struct tms));
	}
	return 0;
}

int SCOPE
replay_times(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();	
	if (eax >= 0) {
		read_mem(regs->ebx, sizeof(struct tms));
	}
	return eax;
}


#else

void
output_times(int nr)
{
	int32_t ret = read_eax();
	if (ret >= 0) {
		skip(sizeof(struct tms));
	}
	printf("times:\t%d\n", ret);
}
#endif

