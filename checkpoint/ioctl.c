#include "syscalls.h"
#include "ioctl.h"
#include "ioctl_tty.h"

#ifndef SYSCALL_PRINTER

int SCOPE
pre_ioctl(const struct syscall_regs * regs)
{
	write_regs(regs);
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
post_ioctl(const struct syscall_regs * regs)
{
	INJ_TRACE("ioctl: fd=%d, cmd=0x%x, arg=0x%x\n",
			regs->ebx, regs->ecx, regs->edx);
	int fd = regs->ebx;
	int cmd = regs->ecx;
	int arg = regs->edx;

	write_eax(regs);

	if (_IOC_TYPE(cmd) == 'T') {
		return post_tty_ioctl(fd, cmd, arg);
	} else {
		INJ_ERROR("no such ioctl command: 0x%x\n", cmd);
		__exit(-1);
	}

	return 0;
}

int SCOPE
replay_ioctl(const struct syscall_regs * regs)
{
	struct syscall_regs saved_regs;
	read_obj(saved_regs);
	/* check */
	ASSERT(saved_regs.eax == regs->eax, "");
	ASSERT(saved_regs.ebx == regs->ebx, "");
	ASSERT(saved_regs.ecx == regs->ecx, "");
	ASSERT(saved_regs.edx == regs->edx, "");

	int fd = regs->ebx;
	int cmd = regs->ecx;
	int arg = regs->edx;

	if (_IOC_TYPE(cmd) == 'T') {
		return replay_tty_ioctl(fd, cmd, arg, regs);
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
	struct syscall_regs r;
	read_regs(r);
	int fd = r.ebx;
	int cmd = r.ecx;
	int arg = r.edx;
	printf("ioctl:\t fd=%d, cmd=0x%x, arg=0x%x\n",
			fd, cmd, arg);

	if (_IOC_TYPE(cmd) == 'T') {
		output_tty_ioctl(fd, cmd, arg);
		return;
	} else {
		INJ_ERROR("no such ioctl command: 0x%x\n", cmd);
		THROW(EXCEPTION_FATAL, "unsupport ioctl cmd %x", cmd);
	}
}
#endif

// vim:ts=4:sw=4

