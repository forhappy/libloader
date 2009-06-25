#include "syscalls.h"


struct old_utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
};

int SCOPE
post_uname(struct syscall_regs * regs)
{
	struct old_utsname utsname;
	write_syscall_nr(__NR_uname);
	write_eax(regs);
	if (regs->eax >= 0) {
		__dup_mem(&utsname, regs->ebx, sizeof(struct old_utsname));
		write_mem(&utsname, sizeof(utsname));
	}
	return 0;
}

