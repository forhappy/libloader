/* 
 * by WN @ Jul. 01, 2009
 */

#include "ioctl_tty.h"

#ifndef SYSCALL_PRINTER

SCOPE int
pre_tty_ioctl(int fd, uint32_t cmd, uint32_t arg)
{
	return 0;
}

SCOPE int
post_tty_ioctl(int fd, uint32_t cmd, uint32_t arg, int32_t retval)
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
	/* first check signal */
	int16_t flag = read_int16();
	if (flag != -1) {
		/* signal distrub me */
		seek_logger(-2, SEEK_END);
		int16_t sig = read_int16();
		INJ_WARNING("ioctl 0x%x distrubed by signal %d, this logger has over. switch a ckpt.\n",
				cmd, -sig);
#ifdef IN_INJECTOR
		replay_trap(regs);
#endif
	}

	/* write eax is done in  */
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
	int16_t sigflag = read_int16();
	if (sigflag != -1) {
		printf("ioctl distrubed by signal\n");
		finished = 1;
		return;
	}
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
		default:
			INJ_WARNING("Unknown tty ioctl cmd 0x%x\n", cmd);
			THROW(EXCEPTION_FATAL, "unsupport ioctl 0x%x", cmd);
	}

	printf("\tretval: %d\n", read_eax());
	return;
}

#endif

