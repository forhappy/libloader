
#include "syscalls.h"


struct k_timespec {
        long       ts_sec;
        long       ts_nsec;
};

int SCOPE
post_nanosleep(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_nanosleep);
	write_eax(regs);
	uintptr_t o = regs->ecx;
	if (o != 0)
		write_mem(o, sizeof(struct k_timespec));
	
	return 0;
}

