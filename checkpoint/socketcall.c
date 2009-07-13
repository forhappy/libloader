

#include "syscalls.h"

#include "socketcall.h"

/* Argument list sizes for sys_socketcall */
#define AL(x) ((x) * sizeof(unsigned long))
static const unsigned char nargs[18]={
	AL(0),AL(3),AL(3),AL(3),AL(2),AL(3),
	AL(3),AL(3),AL(4),AL(4),AL(4),AL(6),
	AL(6),AL(2),AL(5),AL(5),AL(3),AL(3)
};

#undef AL

#ifndef SYSCALL_PRINTER

int SCOPE
post_socketcall(struct syscall_regs * regs)
{
	INJ_VERBOSE("in socket_call\n");

	int call = regs->ebx;
	uint32_t args = regs->ecx;

	if (call < 1 || call > SYS_RECVMSG) {
		INJ_FATAL("No such syscall number: %d\n", call);
		__exit(-1);
	}

	write_obj(call);
	write_eax(regs);
	unsigned long a0, a1, a[6];
	a0 = a[0];
	a1 = a[1];
	uint32_t retval = regs->eax;

	__dup_mem(a, args, nargs[call]);
	write_mem(args, nargs[call]);

	switch (call) {
		case SYS_SOCKET:
			return post_socket(a0, a1, a[2], retval);
		case SYS_BIND:
			return post_bind(a0, a1, a[2], retval);
		case SYS_GETSOCKNAME:
			return post_getsockname(a0, a1, a[2], retval);
		case SYS_SENDTO:
			return post_sendto(a0, a1, a[2], a[3], a[4], a[5], retval);
		case SYS_RECVFROM:
			return post_recvfrom(a0, a1, a[2], a[3], a[4], a[5], retval);
		default:
			INJ_WARNING("Unknown socket call: %d\n", call);
			__exit(-1);
	}

	__exit(-1);
	return 0;
}

#else

void
output_socketcall(void)
{

	int call;
	int retval;

	read_obj(call);
	printf("socketcall:\t%d\n", call);

	if ((call < 1) || (call > SYS_RECVMSG))
		THROW(EXCEPTION_FATAL, "No such socket number: %d\n", call);

	retval = read_eax();
	unsigned long a0, a1, a[6];
	read_mem(a, nargs[call]);
	a0 = a[0];
	a1 = a[1];
	

	switch (call) {
		case SYS_SOCKET:
			output_socket(a0, a1, a[2], retval);
			return;
		case SYS_BIND:
			output_bind(a0, a1, a[2], retval);
			return;
		case SYS_GETSOCKNAME:
			output_getsockname(a0, a1, a[2], retval);
			return;
		case SYS_SENDTO:
			output_sendto(a0, a1, a[2], a[3], a[4], a[5], retval);
			return;
		case SYS_RECVFROM:
			output_recvfrom(a0, a1, a[2], a[3], a[4], a[5], retval);
			return;
		default:
			THROW(EXCEPTION_FATAL, "Unknown socket number: %d", call);
	}
}
#endif

