/* 
 * ioctl_blk.c
 * by WN @ Aug. 12, 2009
 */

#include "ioctl_blk.h"

#ifndef SYSCALL_PRINTER

SCOPE int
post_blk_ioctl(int fd, uint32_t cmd, uint32_t arg,
		const struct syscall_regs * regs)
{
	if (regs->eax < 0)
		return 0;

	if (_IOC_DIR(cmd) == _IOC_READ) {
		ASSERT(arg != 0, regs, "!@$$!@#\n");
		write_mem(arg, _IOC_SIZE(cmd));
	}
	return 0;
}

SCOPE int
replay_blk_ioctl(int fd, uint32_t cmd, uint32_t arg,
	const struct syscall_regs * regs)
{
	/* write eax is done in ioctl.c */
	int32_t eax = read_int32();
	if (eax < 0)
		return eax;

	if (_IOC_DIR(cmd) == _IOC_READ)
		read_mem(arg, _IOC_SIZE(cmd));

	return eax;
}


#else

extern int finished;
void
output_blk_ioctl(int fd, uint32_t cmd, uint32_t arg)
{
	int32_t eax = read_int32();

	printf("\tretval: %d\n", eax);

	if (eax < 0)
		return;

	if (_IOC_DIR(cmd) == _IOC_READ)
		skip(_IOC_SIZE(cmd));
}


#endif

