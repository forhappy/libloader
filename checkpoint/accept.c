
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_accept(int fd, uintptr_t pupeer_sockaddr,
		uintptr_t pupeer_addrlen, int retval, const struct syscall_regs * regs)
{
	if (retval > 0) {
		/* sockaddr */
		if (pupeer_sockaddr == 0) {
			/* write a 0 into log */
			uint32_t v = 0;
			write_obj(v);
			return 0;
		}

		ASSERT(pupeer_addrlen != 0, NULL, "that shouldn't happed...\n");

		/* write addrlen into log */
		uint32_t l;
		__dup_mem(&l, pupeer_addrlen, sizeof(l));
		write_obj(l);
		write_mem(pupeer_sockaddr, l);
	}
	return 0;
}

int SCOPE
replay_accept(int fd, uintptr_t pupeer_sockaddr,
		uintptr_t pupeer_addrlen, int retval, const struct syscall_regs * regs)
{
	if (retval > 0) {
		uint32_t l;
		l = read_uint32();
		if (l == 0)
			return retval;
		ASSERT(pupeer_addrlen != 0, NULL, "shouldn't happen\n");
		__upd_mem(pupeer_addrlen, &l, sizeof(l));
		read_mem(pupeer_sockaddr, l);
	}
	return retval;
}


#else

void SCOPE
output_accept(int fd, uintptr_t pupeer_sockaddr,
		uintptr_t pupeer_addrlen, int retval)
{
	printf("accept(fd=%d, pupeer_sockaddr=0x%x, pupeer_addrlen=0x%x):\t%d\n",
			fd, pupeer_sockaddr, pupeer_addrlen, retval);
	if (retval > 0) {
		uint32_t l = read_uint32();
		if (l != 0)
			skip(l);
	}
}

#endif

