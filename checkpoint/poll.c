
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
	int eax = regs->eax;
	write_eax(regs);
	if (eax > 0) {
		write_mem(ufds, eax * sizeof(struct pollfd));
	}
	return 0;
}

int SCOPE
replay_poll(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	uint32_t ufds = regs->ebx;
	if (eax > 0)
		read_mem(ufds, eax * sizeof(struct pollfd));
	return eax;
}
#else

void
output_poll(void)
{
	int eax = read_eax();
	printf("poll:\t0x%x\n", eax);
	if (eax > 0)
		skip(eax * sizeof(struct pollfd));
}
#endif

