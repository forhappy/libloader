
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_getsockname(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval, const struct syscall_regs * regs)
{
	if (retval < 0)
		return 0;

	uint32_t len;
	__dup_mem(&len, usockaddr_len, sizeof(len));
	write_mem(usockaddr_len, sizeof(len));
	write_mem(usockaddr, len);
	return 0;
}

int SCOPE
replay_getsockname(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval, const struct syscall_regs * regs)
{
	if (retval < 0)
		return retval;

	read_mem(usockaddr_len, sizeof(uint32_t));
	uint32_t len;
	__dup_mem(&len, usockaddr_len, sizeof(len));
	read_mem(usockaddr, len);
	return retval;
}


#else

void SCOPE
output_getsockname(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval)
{
	printf("getsockname(getsockname=%d, pusockaddr=0x%x, pusockaddr_len=0x%x):\t%d\n",
			fd, usockaddr, usockaddr_len, retval);
	if (retval < 0)
		return;

	uint32_t len;
	read_obj(len);
	skip(len);
}

#endif

