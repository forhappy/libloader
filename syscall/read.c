#include "syscall_tbl.h"
#include "log_and_ckpt.h"

int SCOPE
post_read(const struct syscall_regs * regs)
{
	
	write_eax(regs);
	if (regs->eax > 0)
		write_mem(regs->ecx, regs->eax);
	return 0;
}

