#ifndef SOCKETCALL_H
#define SOCKETCALL_H

#include "syscalls.h"

#define SYS_SOCKET	1		/* sys_socket(2)		*/
#define SYS_BIND	2		/* sys_bind(2)			*/
#define SYS_CONNECT	3		/* sys_connect(2)		*/
#define SYS_LISTEN	4		/* sys_listen(2)		*/
#define SYS_ACCEPT	5		/* sys_accept(2)		*/
#define SYS_GETSOCKNAME	6		/* sys_getsockname(2)		*/
#define SYS_GETPEERNAME	7		/* sys_getpeername(2)		*/
#define SYS_SOCKETPAIR	8		/* sys_socketpair(2)		*/
#define SYS_SEND	9		/* sys_send(2)			*/
#define SYS_RECV	10		/* sys_recv(2)			*/
#define SYS_SENDTO	11		/* sys_sendto(2)		*/
#define SYS_RECVFROM	12		/* sys_recvfrom(2)		*/
#define SYS_SHUTDOWN	13		/* sys_shutdown(2)		*/
#define SYS_SETSOCKOPT	14		/* sys_setsockopt(2)		*/
#define SYS_GETSOCKOPT	15		/* sys_getsockopt(2)		*/
#define SYS_SENDMSG	16		/* sys_sendmsg(2)		*/
#define SYS_RECVMSG	17		/* sys_recvmsg(2)		*/

extern SCOPE int
post_socket(int family, int type, int protocol, int retval);

extern void
output_socket(int family, int type, int protocol, int retval);

extern int SCOPE
post_bind(int fd, uint32_t umyaddr, int addrlen, int retval);

extern void SCOPE
output_bind(int fd, uint32_t umyaddr, int addrlen, int retval);

extern int SCOPE
post_getsockname(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval);

extern void SCOPE
output_getsockname(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval);

extern int SCOPE
post_sendto(int fd, uint32_t buff, uint32_t len, uint32_t flags,
		uint32_t addr, uint32_t addr_len, int retval);

extern void SCOPE
output_sendto(int fd, uint32_t buff, uint32_t len, uint32_t flags,
		uint32_t addr, uint32_t addr_len, int retval);

extern int SCOPE
post_recvfrom(int fd, uint32_t ubuf, uint32_t size,
		uint32_t flags, uint32_t addr,
		uint32_t addr_len, int retval);

extern void SCOPE
output_recvfrom(int fd, uint32_t ubuf, uint32_t size,
		uint32_t flags, uint32_t addr,
		uint32_t addr_len, int retval);

extern int SCOPE
post_recvmsg(int fd, uint32_t msg, uint32_t flags, int retval);

extern void SCOPE
output_recvmsg(int fd, uint32_t msg, uint32_t flags, int retval);

extern int SCOPE
post_connect(int fd, uint32_t uservaddr, int addrlen, int retval);

extern void SCOPE
output_connect(int fd, uint32_t uservaddr, int addrlen, int retval);

extern int SCOPE
post_recv(int fd, uint32_t ubuf, uint32_t size, uint32_t flags, int retval);

extern void SCOPE
output_recv(int fd, uint32_t ubuf, uint32_t size, uint32_t flags, int retval);

extern int SCOPE
post_getpeername(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval);

extern void SCOPE
output_getpeername(int fd, uint32_t usockaddr, uint32_t usockaddr_len, int retval);

#endif 

