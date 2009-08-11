
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_recvfrom(int fd, uint32_t ubuf, uint32_t size,
		uint32_t flags, uint32_t addr,
		uint32_t addr_len, int retval, const struct syscall_regs * regs)
{
	/* if addr and addr_len error, retval < 0, but
	 * data will still be copied into buffer */
	write_mem(ubuf, size);
	if (retval >= 0) {
		if (addr != 0) {
			if (addr_len == 0) {
				INJ_WARNING("that shouldn't happen\n");
				__exit(-1);
			}
			uint32_t l;
			__dup_mem(&l, addr_len, sizeof(l));
			write_obj(l);
			write_mem(addr, l);
		}
	}
	return 0;
}

int SCOPE
replay_recvfrom(int fd, uint32_t ubuf, uint32_t size,
		uint32_t flags, uint32_t addr,
		uint32_t addr_len, int retval, const struct syscall_regs * regs)
{
	read_mem(ubuf, size);
	if (retval >= 0) {
		if (addr != 0) {
			if (addr_len == 0) {
				INJ_WARNING("that shouldn't happen\n");
				__exit(-1);
			}

			read_mem(addr_len, sizeof(uint32_t));
			uint32_t l;
			__dup_mem(&l, addr_len, sizeof(l));
			read_mem(addr, l);
		}
	}
	return retval;
}


#else

void SCOPE
output_recvfrom(int fd, uint32_t ubuf, uint32_t size,
		uint32_t flags, uint32_t addr,
		uint32_t addr_len, int retval)
{
	printf("recvfrom(fd=%d, ubuf=0x%x, size=%d, flags=0x%x, addr=0x%x, addr_len=%d):\t%d\n",
			fd, ubuf, size, flags, addr, addr_len, retval);
	skip(size);
	if (retval >= 0) {
		if (addr != 0) {
			uint32_t l;
			read_obj(l);
			skip(l);
		}
	}
}

#endif

