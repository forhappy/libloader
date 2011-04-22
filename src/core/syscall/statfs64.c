
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
#include <asm/statfs.h>

int SCOPE
post_statfs64(const struct syscall_regs * regs)
{
	write_eax(regs);
#if 0
	if (regs->eax >= 0) {
		write_mem(regs->ecx, sizeof(struct statfs64));
	}
#endif
	return 0;
}

