

#include "syscalls.h"

#include "socketcall.h"

/* memcmp */
#ifdef IN_INJECTOR
# include "../libwrapper/string_32.h"
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

	int sz = nargs[call];

	__dup_mem(a, args, sz);
	write_mem(args, sz);

	a0 = a[0];
	a1 = a[1];
	a2 = a[2];

	switch (call) {
		case SYS_SOCKET:
			return post_socket(a0, a1, a2, retval, regs);
		case SYS_BIND:
			return post_bind(a0, a1, a2, retval, regs);
		case SYS_GETSOCKNAME:
			return post_getsockname(a0, a1, a2, retval, regs);
		case SYS_SENDTO:
			return post_sendto(a0, a1, a2, a[3], a[4], a[5], retval, regs);
		case SYS_RECVFROM:
			return post_recvfrom(a0, a1, a2, a[3], a[4], a[5], retval, regs);
		case SYS_RECVMSG:
			return post_recvmsg(a0, a1, a2, retval, regs);
		case SYS_CONNECT:
			return post_connect(a0, a1, a2, retval, regs);
		case SYS_RECV:
			return post_recv(a0, a1, a2, a[3], retval, regs);
		case SYS_GETPEERNAME:
			return post_getpeername(a0, a1, a2, retval, regs);
		case SYS_SETSOCKOPT:
			return post_setsockopt(a0, a1, a2, a[3], a[4], retval, regs);
		case SYS_LISTEN:
			return post_listen(a0, a1, retval, regs);
		case SYS_ACCEPT:
			return post_accept(a0, a1, a2, retval, regs);
		case SYS_SHUTDOWN:
			return post_shutdown(a0, a1, retval, regs);
		case SYS_SEND:
			return post_send(a0, a1, a2, a[3], retval, regs);
		case SYS_SOCKETPAIR:
			return post_socketpair(a0, a1, a2, a[3], retval, regs);
		case SYS_SENDMSG:
			return post_sendmsg(a0, a1, a2, retval, regs);
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
	ASSERT(call == regs->ebx, regs, "");
	uint32_t args = regs->ecx;

	unsigned long a0, a1, a2, a[6];

	uint32_t retval = eax;
	int sz = nargs[call];
	read_mem(a, sz);
	INJ_TRACE("replay socketcall 0x%x\n", call);
	ASSERT(memcmp(a, (void*)args, sz) == 0, regs, "!@!@#\n");

	a0 = a[0];
	a1 = a[1];
	a2 = a[2];


	switch (call) {
		case SYS_SOCKET:
			replay_socket(a0, a1, a2, retval, regs);
			break;
		case SYS_BIND:
			replay_bind(a0, a1, a2, retval, regs);
			break;
		case SYS_GETSOCKNAME:
			replay_getsockname(a0, a1, a2, retval, regs);
			break;
		case SYS_SENDTO:
			replay_sendto(a0, a1, a2, a[3], a[4], a[5], retval, regs);
			break;
		case SYS_RECVFROM:
			replay_recvfrom(a0, a1, a2, a[3], a[4], a[5], retval, regs);
			break;
		case SYS_RECVMSG:
			replay_recvmsg(a0, a1, a2, retval, regs);
			break;
		case SYS_CONNECT:
			replay_connect(a0, a1, a2, retval, regs);
			break;
		case SYS_RECV:
			replay_recv(a0, a1, a2, a[3], retval, regs);
			break;
		case SYS_GETPEERNAME:
			replay_getpeername(a0, a1, a2, retval, regs);
			break;
		case SYS_SETSOCKOPT:
			replay_setsockopt(a0, a1, a2, a[3], a[4], retval, regs);
			break;
		case SYS_LISTEN:
			replay_listen(a0, a1, retval, regs);
			break;
		case SYS_ACCEPT:
			replay_accept(a0, a1, a2, retval, regs);
			break;
		case SYS_SHUTDOWN:
			replay_shutdown(a0, a1, retval, regs);
			break;
		case SYS_SEND:
			replay_send(a0, a1, a2, a[3], retval, regs);
			break;
		case SYS_SOCKETPAIR:
			replay_socketpair(a0, a1, a2, a[3], retval, regs);
			break;
		case SYS_SENDMSG:
			replay_sendmsg(a0, a1, a2, retval, regs);
			break;
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
	return retval;
}



#else

void
output_socketcall(int nr)
{

	int call;
	int retval;

	read_obj(call);
	printf("socketcall:\t%d\n", call);

	if ((call < 1) || (call > SYS_RECVMSG))
		THROW(EXCEPTION_FATAL, "No such socket number: %d\n", call);

	retval = read_eax();
	unsigned long a0, a1, a2, a[6];
	int sz = nargs[call];
	read_mem(a, sz);
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
			output_setsockopt(a0, a1, a2, a[3], a[4], retval);
			return;
		case SYS_LISTEN:
			output_listen(a0, a1, retval);
			return;
		case SYS_ACCEPT:
			output_accept(a0, a1, a2, retval);
			return;
		case SYS_SHUTDOWN:
			output_shutdown(a0, a1, retval);
			return;
		case SYS_SEND:
			output_send(a0, a1, a2, a[3], retval);
			return;
		case SYS_SOCKETPAIR:
			output_socketpair(a0, a1, a2, a[3], retval);
			return;
		case SYS_SENDMSG:
			output_sendmsg(a0, a1, a2, retval);
			return;
		default:
			THROW(EXCEPTION_FATAL, "Unknown socket number: %d", call);
	}
}
#endif

