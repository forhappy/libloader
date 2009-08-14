/* 
 * by WN @ Jul. 01, 2009
 */

#include "ioctl_tty.h"

#ifndef SYSCALL_PRINTER

SCOPE int
post_tty_ioctl(int fd, uint32_t cmd, uint32_t arg,
		const struct syscall_regs * regs)
{
	switch (cmd) {
		case TCGETS:
			if (arg != 0)
				write_mem(arg, sizeof(struct termios));
			return 0;
		case TIOCGWINSZ:
			if (arg != 0)
				write_mem(arg, sizeof(struct winsize));
			return 0;
		case FIONREAD:
			if (arg != 0)
				write_mem(arg, sizeof(int));
			return 0;
		case FIONBIO:
			return 0;
		default:
			INJ_WARNING("doesn't know such tty ioctl: 0x%x\n", cmd);
			__exit(-1);
	}
	return 0;
}


SCOPE int
replay_tty_ioctl(int fd, uint32_t cmd, uint32_t arg,
	const struct syscall_regs * regs)
{
	/* write eax is done in ioctl.c */
	int32_t eax = read_int32();
	switch (cmd) {
		case TCGETS:
			if (arg != 0)
				read_mem(arg, sizeof(struct termios));
			return eax;
		case TIOCGWINSZ:
			if (arg != 0)
				read_mem(arg, sizeof(struct winsize));
			return eax;
		case FIONREAD:
			if (arg != 0)
				read_mem(arg, sizeof(int));
			return eax;
		case FIONBIO:
			return eax;
		default:
			INJ_WARNING("doesn't know such tty ioctl: 0x%x\n", cmd);
			__exit(-1);
	}
	return eax;
}



#else

extern int finished;
void
output_tty_ioctl(int fd, uint32_t cmd, uint32_t arg)
{
	printf("\tretval: %d\n", read_eax());

	switch (cmd) {
		case TCGETS:
			if (arg != 0)
				skip(sizeof(struct termios));
			break;
		case TIOCGWINSZ:
			if (arg != 0)
				skip(sizeof(struct winsize));
			break;
		case FIONREAD:
			if (arg != 0)
				skip(sizeof(int));
			break;
		case FIONBIO:
			break;
		default:
			INJ_WARNING("Unknown tty ioctl cmd 0x%x\n", cmd);
			THROW(EXCEPTION_FATAL, "unsupport ioctl 0x%x", cmd);
	}

	return;
}

#endif

