#include "syscall_tbl.h"
#include "log_and_ckpt.h"


struct old_utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
};


int SCOPE
post_uname(const struct syscall_regs * regs)
{
	
	write_eax(regs);
	if (regs->eax >= 0) {
		write_mem(regs->ebx, sizeof(struct old_utsname));
	}
	return 0;
}

