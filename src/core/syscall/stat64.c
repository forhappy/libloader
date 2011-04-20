
#include "syscall_tbl.h"
#include "log_and_ckpt.h"


int SCOPE
post_stat64(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		write_mem(regs->ecx, sizeof(struct stat64));
	}
	return 0;
}

