
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_sendto(int fd, uint32_t buff, uint32_t len, uint32_t flags,
		uint32_t addr, uint32_t addr_len, int retval)
{
	return 0;
}

int SCOPE
replay_sendto(int fd, uint32_t buff, uint32_t len, uint32_t flags,
		uint32_t addr, uint32_t addr_len, int retval)
{
	return retval;
}


#else

void SCOPE
output_sendto(int fd, uint32_t buff, uint32_t len, uint32_t flags,
		uint32_t addr, uint32_t addr_len, int retval)
{
	printf("sendto(fd=%d, buff=0x%x, len=%d, flags=0x%x, addr=0x%x, addr_len=%d):\t%d\n",
			fd, buff, len, flags, addr, addr_len, retval);
}

#endif

