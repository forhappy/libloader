/* 
 * sockpair.c
 * by WN @ Jun. 11, 2010
 *
 * implement sockpair interface in host code
 *
 */

#include <common/defs.h>
#include <common/debug.h>
#include <common/replay/socketpair.h>
#include <host/exception.h>
#include <sys/socket.h>
#include <assert.h>

void
sock_send(void * data, size_t len)
{
	assert(data != NULL);
	int err;
	while (len > 0) {
		err = send(HOST_SOCKPAIR_FD, data, len, 0);
		if (err < 0)
			THROW_FATAL(EXP_SOCKPAIR_ERROR, "error %d when send data",
					err);
		len -= err;
		data += err;
	}
}

void
sock_recv(void * data, size_t len)
{
	assert(data != NULL);
	int err;
	while (len > 0) {
		err = recv(HOST_SOCKPAIR_FD, data, len, MSG_WAITALL);
		if (err < 0)
			THROW_FATAL(EXP_SOCKPAIR_ERROR, "error %d when receive data",
					err);
		len -= err;
		data += err;
	}
}

// vim:ts=4:sw=4

