
#include "syscalls.h"


struct epoll_event {
	uint32_t events;
	uint64_t data;
} EPOLL_PACKED;

#ifndef SYSCALL_PRINTER

#ifdef __x86_64__
#define EPOLL_PACKED __attribute__((packed))
#else
#define EPOLL_PACKED
#endif

int SCOPE
post_epoll_wait(const struct syscall_regs * regs)
{
	int eax = regs->eax;
	uint32_t events = regs->ecx;
	uint32_t maxevents = regs->edx;

	write_eax(regs);

	if (eax > 0) {
		ASSERT(eax <= maxevents, regs, "!#$@#@!\n");
		write_mem(events, sizeof(struct epoll_event) * eax);
	}
	return 0;
}

int SCOPE
replay_epoll_wait(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	uint32_t events = regs->ecx;
	uint32_t maxevents = regs->edx;
	
	if (eax > 0) {
		ASSERT(eax <= maxevents, regs, "!#$@#@!\n");
		read_mem(events, eax * sizeof(struct epoll_event));
	}
	return eax;
}
#else

void
output_epoll_wait(int nr)
{
	int eax = read_eax();
	printf("epoll_wait:\t0x%x\n", eax);
	if (eax > 0)
		skip(sizeof(struct epoll_event) * eax);
}
#endif

