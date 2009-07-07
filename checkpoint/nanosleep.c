
#include "syscalls.h"


struct k_timespec {
        long       ts_sec;
        long       ts_nsec;
};


#ifndef SYSCALL_PRINTER

int SCOPE
post_nanosleep(struct syscall_regs * regs)
{
	
	write_eax(regs);
	uintptr_t o = regs->ecx;
	if (o != 0)
		write_mem(o, sizeof(struct k_timespec));
	
	return 0;
}

#else

void
output_nanosleep(void)
{
	
}
#endif

