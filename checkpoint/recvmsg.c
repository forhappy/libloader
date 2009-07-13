
#include "socketcall.h"

struct iovec
{
	uint32_t iov_base;
	uint32_t iov_len;
};

struct msghdr {
	uint32_t msg_name;	/* Socket name			*/
	uint32_t msg_namelen;	/* Length of name		*/
	uint32_t msg_iov;	/* Data blocks			*/
	uint32_t msg_iovlen;	/* Number of blocks		*/
	uint32_t msg_control;	/* Per protocol magic (eg BSD file descriptor passing) */
	uint32_t msg_controllen;	/* Length of cmsg list */
	uint32_t msg_flags;
};

#define SZ_IOVS	(sizeof(struct iovec) * iov_len)

#ifndef SYSCALL_PRINTER

int SCOPE
post_recvmsg(int fd, uint32_t msg, uint32_t flags, int retval)
{
	/* don't check retval, save everything */

	struct msghdr hdr;
	hdr.msg_name = 0;
	__dup_mem(&hdr, msg, sizeof(hdr));
	write_obj(hdr);

	if (hdr.msg_name != 0)
		write_mem(hdr.msg_name, hdr.msg_namelen);

	/* iovs */
	struct iovec * vecs;
	int iov_len = hdr.msg_iovlen;
#ifdef IN_INJECTOR
	vecs = (void*)hdr.msg_iov;
#else
	vecs = alloca(SZ_IOVS);
	__dup_mem(vecs, hdr.msg_iov, SZ_IOVS);
#endif
	write_mem(hdr.msg_iov, SZ_IOVS);
	/* for each iov, save the data */
	for (int i = 0; i < iov_len; i++)
		write_mem(vecs[i].iov_base, vecs[i].iov_len);
	return 0;
}

#else

void SCOPE
output_recvmsg(int fd, uint32_t msg, uint32_t flags, int retval)
{
	printf("recvmsg(fd=%d, msg=0x%x, flags=0x%x):%d\n",
			fd, msg, flags, retval);
	struct msghdr hdr;
	read_obj(hdr);

	if (hdr.msg_name != 0)
		skip(hdr.msg_namelen);

	int iov_len = hdr.msg_iovlen;
	struct iovec * vecs = alloca(SZ_IOVS);
	read_mem(vecs, SZ_IOVS);

	for (int i = 0; i < iov_len; i++)
		skip(vecs[i].iov_len);
}

#endif

