/* 
 * sockpair.h
 * by WN @ Jun. 10, 2010
 */

#ifndef SN_SOCKPAIR_H
#define SN_SOCKPAIR_H

/* this file is introduced to define socket pair which is used
 * in communication between gdbserver and target process. We will
 * explain sockpair in detail below.
 *
 * During replay, the target process and the gdbserver needs to
 * communicate with each other. For example, when executing system call,
 * the data is read by gdbserver and send to client using socket.
 * However, we still use signal driving process, that is, each time client
 * requires socket transmission, it kill itself using a SIGRTxx signal.
 *
 * this file define some macro used by socket transmission.
 *
 * */

/* this is the socket pair array, [0] is used in gdbserver(host),
 * [1] is (temporary) used in target (before execve) */
extern int socket_pair_fds[2];

#define HOST_SOCKPAIR_FD	(socket_pair_fds[0])
#define TARGET_SOCKPAIR_FD_TEMP	(socket_pair_fds[1])

/* TARGET_SOCKPAIR_FD is a fixed number used after execution. This 
 * design decision can simplify the libinterp's code. */
#define TARGET_SOCKPAIR_FD	(94)

extern void
sock_send(void * data, size_t len);

extern void
sock_recv(void * data, size_t len);

#endif

// vim:ts=4:sw=4

