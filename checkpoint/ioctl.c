#include "syscalls.h"
#include "ioctl.h"
#include "ioctl_tty.h"

#ifndef SYSCALL_PRINTER

int SCOPE
pre_ioctl(struct syscall_regs * regs)
{
	INJ_TRACE("ioctl: fd=%d, cmd=0x%x, arg=0x%x\n",
			regs->ebx, regs->ecx, regs->edx);
	int fd = regs->ebx;
	int cmd = regs->ecx;
	int arg = regs->edx;

	if (_IOC_TYPE(cmd) == 'T') {
		return pre_tty_ioctl(fd, cmd, arg);
	} else {
		INJ_ERROR("no such ioctl command: 0x%x\n", cmd);
		__exit(-1);
	}

	return 0;
}

int SCOPE
post_ioctl(struct syscall_regs * regs)
{
	INJ_TRACE("ioctl: fd=%d, cmd=0x%x, arg=0x%x\n",
			regs->ebx, regs->ecx, regs->edx);
	int fd = regs->ebx;
	int cmd = regs->ecx;
	int arg = regs->edx;

	if (_IOC_TYPE(cmd) == 'T') {
		return post_tty_ioctl(fd, cmd, arg);
	} else {
		INJ_ERROR("no such ioctl command: 0x%x\n", cmd);
		__exit(-1);
	}

	return 0;
}

#else

void
output_ioctl(void)
{
	
}
#endif

// vim:ts=4:sw=4

