
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
int SCOPE
post_write(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}
