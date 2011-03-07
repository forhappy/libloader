
#include "syscalls.h"

struct pollfd {
	int fd;
	short events;
	short revents;
};

#ifndef SYSCALL_PRINTER

int SCOPE
post_poll(const struct syscall_regs * regs)
{
	uint32_t ufds = regs->ebx;
	uint32_t nfds = regs->ecx;
	int eax = regs->eax;

	write_eax(regs);

	if (eax > 0) {
		write_obj(nfds);
		write_mem(ufds, nfds * sizeof(struct pollfd));
	}
	return 0;
}

int SCOPE
replay_poll(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	uint32_t ufds = regs->ebx;
	uint32_t nfds;
	
	if (eax > 0) {
		read_obj(nfds);
		ASSERT(nfds == regs->ecx, regs, "poll nfds inconsistent\n");

		read_mem(ufds, nfds * sizeof(struct pollfd));
	}
	return eax;
}
#else

void
output_poll(int nr)
{
	int eax = read_eax();
	int i = 0;
	printf("poll:\t0x%x\n", eax);
	if (eax > 0) {
		/* for each struct pollfd, read and print */
		int nfds = read_int32();

		for (i = 0; i < nfds; i++) {
			struct pollfd s;
			read_obj(s);
			printf("\tfd=%d, events=%d, revents=%d\n",
					s.fd, s.events, s.revents);
		}
	}
}
#endif

