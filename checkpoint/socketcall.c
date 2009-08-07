

#include "syscalls.h"

#include "socketcall.h"

/* memcmp */
#ifdef IN_INJECTOR
# include "string_32.h"
#else
# define memcmp __builtin_memcmp
#endif

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
post_socketcall(const struct syscall_regs * regs)
{

	int call = regs->ebx;
	uint32_t args = regs->ecx;

	INJ_TRACE("in socket_call, nr=0x%x\n", call);
	if (call < 1 || call > SYS_RECVMSG) {
		INJ_FATAL("No such syscall number: %d\n", call);
		__exit(-1);
	}

	write_obj(call);
	write_eax(regs);
	unsigned long a0, a1, a2, a[6];
	uint32_t retval = regs->eax;

	__dup_mem(a, args, nargs[call]);
	write_mem(args, nargs[call]);

	a0 = a[0];
	a1 = a[1];
	a2 = a[2];

	switch (call) {
		case SYS_SOCKET:
			return post_socket(a0, a1, a2, retval);
		case SYS_BIND:
			return post_bind(a0, a1, a2, retval);
		case SYS_GETSOCKNAME:
			return post_getsockname(a0, a1, a2, retval);
		case SYS_SENDTO:
			return post_sendto(a0, a1, a2, a[3], a[4], a[5], retval);
		case SYS_RECVFROM:
			return post_recvfrom(a0, a1, a2, a[3], a[4], a[5], retval);
		case SYS_RECVMSG:
			return post_recvmsg(a0, a1, a2, retval);
		case SYS_CONNECT:
			return post_connect(a0, a1, a2, retval);
		case SYS_RECV:
			return post_recv(a0, a1, a2, a[3], retval);
		case SYS_GETPEERNAME:
			return post_getpeername(a0, a1, a2, retval);
		case SYS_SETSOCKOPT:
			return post_setsockopt(a0, a1, a2, a[3], a[4], retval);
		case SYS_LISTEN:
			return post_listen(a0, a1, retval);
		case SYS_ACCEPT:
			return post_accept(a0, a1, a2, retval);
		default:
			INJ_WARNING("Unknown socket call: %d\n", call);
			__exit(-1);
	}

	__exit(-1);
	return 0;
}

int SCOPE
replay_socketcall(const struct syscall_regs * regs)
{
	int call = read_int32();
	int eax = read_int32();
	ASSERT(call == regs->ebx, "");
	uint32_t args = regs->ecx;

	unsigned long a0, a1, a2, a[6];

	uint32_t retval = eax;
	read_mem(a, nargs[call]);
	INJ_TRACE("replay socketcall 0x%x\n", call);
	ASSERT(memcmp(a, (void*)args, nargs[call]) == 0, "!@!@#\n");

	a0 = a[0];
	a1 = a[1];
	a2 = a[2];


	switch (call) {
		case SYS_SOCKET:
			return replay_socket(a0, a1, a2, retval);
		case SYS_BIND:
			return replay_bind(a0, a1, a2, retval);
		case SYS_GETSOCKNAME:
			return replay_getsockname(a0, a1, a2, retval);
		case SYS_SENDTO:
			return replay_sendto(a0, a1, a2, a[3], a[4], a[5], retval);
		case SYS_RECVFROM:
			return replay_recvfrom(a0, a1, a2, a[3], a[4], a[5], retval);
		case SYS_RECVMSG:
			return replay_recvmsg(a0, a1, a2, retval);
		case SYS_CONNECT:
			return replay_connect(a0, a1, a2, retval);
		case SYS_RECV:
			return replay_recv(a0, a1, a2, a[3], retval);
		case SYS_GETPEERNAME:
			return replay_getpeername(a0, a1, a2, retval);
		case SYS_SETSOCKOPT:
			return replay_setsockopt(a0, a1, a2, a[3], a[4], retval);
		case SYS_LISTEN:
			return replay_listen(a0, a1, retval);
		case SYS_ACCEPT:
			return replay_accept(a0, a1, a2, retval);
		default:
			INJ_FATAL("Unknown socket call: %d\n", call);
			INJ_FATAL("eip=0x%x\n", regs->eip);
			INJ_FATAL("esp=0x%x\n", regs->esp);
			INJ_FATAL("ebp=0x%x\n", regs->ebp);
			/* see replay_syscall, int3 traps gdb */
			asm volatile (
				"movl %0, %%esp\n"
				"movl %1, %%ebp\n" : : "m" (regs->esp), "m" (regs->ebp));
			asm volatile ("int3\n");
			__exit(-1);
	}

	__exit(-1);
	return eax;
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
	unsigned long a0, a1, a2, a[6];
	read_mem(a, nargs[call]);
	a0 = a[0];
	a1 = a[1];
	a2 = a[2];
	

	switch (call) {
		case SYS_SOCKET:
			output_socket(a0, a1, a2, retval);
			return;
		case SYS_BIND:
			output_bind(a0, a1, a2, retval);
			return;
		case SYS_GETSOCKNAME:
			output_getsockname(a0, a1, a2, retval);
			return;
		case SYS_SENDTO:
			output_sendto(a0, a1, a2, a[3], a[4], a[5], retval);
			return;
		case SYS_RECVFROM:
			output_recvfrom(a0, a1, a2, a[3], a[4], a[5], retval);
			return;
		case SYS_RECVMSG:
			output_recvmsg(a0, a1, a2, retval);
			return;
		case SYS_CONNECT:
			output_connect(a0, a1, a2, retval);
			return;
		case SYS_RECV:
			output_recv(a0, a1, a2, a[3], retval);
			return;
		case SYS_GETPEERNAME:
			output_getpeername(a0, a1, a2, retval);
			return;
		case SYS_SETSOCKOPT:
			return output_setsockopt(a0, a1, a2, a[3], a[4], retval);
		case SYS_LISTEN:
			return output_listen(a0, a1, retval);
		case SYS_ACCEPT:
			return output_accept(a0, a1, a2, retval);
		default:
			THROW(EXCEPTION_FATAL, "Unknown socket number: %d", call);
	}
}
#endif

