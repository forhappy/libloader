#include "syscall_tbl.h"
#include "../debug.h"
#include "log_and_ckpt.h"
int SCOPE
post_ioctl(const struct syscall_regs * regs)
{
	VERBOSE(LOG_SYSCALL, "ioctl: fd=%d, cmd=0x%x, arg=0x%x\n",
			regs->ebx, regs->ecx, regs->edx);
	write_regs(regs);

	write_eax(regs);
	return 0;
}
// vim:ts=4:sw=4

