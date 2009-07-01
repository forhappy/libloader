/* 
 * by WN @ Jul. 01, 2009
 */

#include "ioctl_tty.h"

SCOPE int
pre_tty_ioctl(int fd, uint32_t cmd, uint32_t arg)
{
	return 0;
}

SCOPE int
post_tty_ioctl(int fd, uint32_t cmd, uint32_t arg)
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
		default:
			INJ_WARNING("doesn't know such tty ioctl: 0x%x\n", cmd);
			__exit(-1);
	}
	return 0;
}


