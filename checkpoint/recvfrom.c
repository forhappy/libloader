
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_recvfrom(int fd, uint32_t ubuf, uint32_t size,
		uint32_t flags, uint32_t addr,
		uint32_t paddr_len, int retval, const struct syscall_regs * regs)
{
	/* if addr and paddr_len error, retval < 0, but
	 * data will still be copied into buffer */
	/* however, if the size too large, this will make logger file
	 * size increase too quickly. */
	if (retval >= 0) {
		write_mem(ubuf, retval);
		if (addr != 0) {
			ASSERT(paddr_len != 0, regs, "that shouldn't happen\n");

			uint32_t l;
			__dup_mem(&l, paddr_len, sizeof(l));
			write_obj(l);
			write_mem(addr, l);
		}
	} else {
		/* we can write garbage data here */
	}
	return 0;
}

int SCOPE
replay_recvfrom(int fd, uint32_t ubuf, uint32_t size,
		uint32_t flags, uint32_t addr,
		uint32_t paddr_len, int retval, const struct syscall_regs * regs)
{
	if (retval >= 0) {
		read_mem(ubuf, retval);
		if (addr != 0) {
			ASSERT(paddr_len != 0, regs, "that shouldn't happen\n");
			read_mem(paddr_len, sizeof(uint32_t));
			uint32_t l;
			__dup_mem(&l, paddr_len, sizeof(l));
			read_mem(addr, l);
		}
	}
	return retval;
}


#else

void SCOPE
output_recvfrom(int fd, uint32_t ubuf, uint32_t size,
		uint32_t flags, uint32_t addr,
		uint32_t paddr_len, int retval)
{
	printf("recvfrom(fd=%d, ubuf=0x%x, size=%d, flags=0x%x, addr=0x%x, paddr_len=0x%x):\t%d\n",
			fd, ubuf, size, flags, addr, paddr_len, retval);
	if (retval >= 0) {
		skip(retval);
		if (addr != 0) {
			uint32_t l;
			read_obj(l);
			skip(l);
		}
	}
}

#endif

