
#include "syscall_tbl.h"
#include "log_and_ckpt.h"

struct k_timespec {
        long       ts_sec;
        long       ts_nsec;
};

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

