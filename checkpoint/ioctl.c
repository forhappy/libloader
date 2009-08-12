#include "syscalls.h"
#include "ioctl.h"
#include "ioctl_tty.h"
#include "ioctl_blk.h"

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

	switch (_IOC_TYPE(cmd)) {
		case 'T':
			return pre_tty_ioctl(fd, cmd, arg);
		case 0x12:
			return pre_blk_ioctl(fd, cmd, arg);
		default:
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

	switch (_IOC_TYPE(cmd)) {
		case 'T':
			return post_tty_ioctl(fd, cmd, arg, regs);
		case 0x12:
			return post_blk_ioctl(fd, cmd, arg, regs);
		default:
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
	ASSERT(saved_regs.eax == regs->eax, regs, "");
	ASSERT(saved_regs.ebx == regs->ebx, regs, "");
	ASSERT(saved_regs.ecx == regs->ecx, regs, "");
	ASSERT(saved_regs.edx == regs->edx, regs, "");

	int fd = regs->ebx;
	int cmd = regs->ecx;
	int arg = regs->edx;

	switch (_IOC_TYPE(cmd)) {
		case 'T':
			return replay_tty_ioctl(fd, cmd, arg, regs);
		case 0x12:
			return replay_blk_ioctl(fd, cmd, arg, regs);
		default:
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

	switch (_IOC_TYPE(cmd)) {
		case 'T':
			return output_tty_ioctl(fd, cmd, arg);
		case 0x12:
			return output_blk_ioctl(fd, cmd, arg);
		default:
			INJ_ERROR("no such ioctl command: 0x%x\n", cmd);
			__exit(-1);
	}
}

#endif
// vim:ts=4:sw=4

