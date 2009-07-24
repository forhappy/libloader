
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_getpeername(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval)
{
	if (retval >= 0) {
		if (usockaddr != 0) {
			if (usockaddr_len == 0) {
				INJ_WARNING("that shouldn't happen\n");
				__exit(-1);
			}
			uint32_t l;
			__dup_mem(&l, usockaddr_len, sizeof(l));
			write_obj(l);
			write_mem(usockaddr, l);
		}
	}
	return 0;
}

int SCOPE
replay_getpeername(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval)
{
	if (retval >= 0) {
		if (usockaddr != 0) {
			if (usockaddr_len == 0) {
				INJ_WARNING("that shouldn't happen\n");
				__exit(-1);
			}
			uint32_t l;
			read_obj(usockaddr_len);
			__dup_mem(&l, usockaddr_len, sizeof(l));
			read_mem(usockaddr, l);
		}
	}
	return 0;
}


#else

void SCOPE
output_getpeername(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval)
{
	printf("getpeername(fd=%d, usockaddr=0x%x, usockaddr_len=0x%x):\t%d\n",
			fd, usockaddr, usockaddr_len, retval);
	if (retval >= 0) {
		if (usockaddr != 0) {
			uint32_t l;
			read_obj(l);
			skip(l);
		}
	}
}

#endif

