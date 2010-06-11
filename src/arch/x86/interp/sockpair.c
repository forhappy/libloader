/* 
 * sockpair.c
 * by WN @ Jun. 10, 2010
 *
 * implement common/replay/sockpair.h
 *
 */
#include <common/defs.h>
#include <common/debug.h>
#include <common/replay/socketpair.h>

#include <xasm/syscall.h>

#ifndef SOCKOP_send
# define SOCKOP_send	(9)
#endif
#ifndef SOCKOP_recv
# define SOCKOP_recv	(10)
#endif
#ifndef MSG_WAITALL
# define MSG_WAITALL (0x100)
#endif

static int
__send(int fd, void * data, size_t len, int flag)
{
	unsigned long paras[4] = {
		[0] = (unsigned long)fd,
		[1] = (unsigned long)data,
		[2] = (unsigned long)len,
		[3] = (unsigned long)flag,
	};
	return INTERNAL_SYSCALL_int80(socketcall, 2,
			SOCKOP_send, paras);
}

static int
__recv(int fd, void * data, size_t len, int flag)
{
	unsigned long paras[4] = {
		[0] = (unsigned long)fd,
		[1] = (unsigned long)data,
		[2] = (unsigned long)len,
		[3] = (unsigned long)flag,
	};
	return INTERNAL_SYSCALL_int80(socketcall, 2,
			SOCKOP_recv, paras);
}


void
sock_send(void * data, size_t len)
{
	assert(data != NULL);
	while (len > 0) {
		int err = __send(TARGET_SOCKPAIR_FD, data, len, 0);
		assert(err > 0);
		len -= err;
		data += err;
	}
}

void
sock_recv(void * data, size_t len)
{
	assert(data != NULL);
	while (len > 0) {
		int err = __recv(TARGET_SOCKPAIR_FD, data, len, MSG_WAITALL);
		assert(err > 0);
		len -= err;
		data += err;
	}
}

// vim:ts=4:sw=4

