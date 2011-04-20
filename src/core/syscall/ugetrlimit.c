
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
#if 0
struct rlimit {
	unsigned long	rlim_cur;
	unsigned long	rlim_max;
};
#endif

/* ugetrlimit is actually getrlimit in kernel,
 * libc's getrlimit is old_getrlimit in kernel  */

int SCOPE
post_ugetrlimit(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax == 0) {
		write_mem(regs->ecx, sizeof(struct rlimit));
	}
	return 0;
}

