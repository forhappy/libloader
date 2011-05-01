/* 
 * xasm/signal.h
 * by WN @ Oct. 24, 2010
 */

#ifndef INTERP_XASM_SIGNAL_H
#define INTERP_XASM_SIGNAL_H

#include <linux/signal.h>

#define GDBSERVER_NOTIFICATION	(SIGRTMAX - 1)

/* SIGRT1 used to support LinuxThread, which "has been superseded by the Native
 * POSIX Thread Library (NPTL)". In current glibc implementation, rt_sigprocmask
 * and sigaction never block SIGRT1. so we can use it to replace SIGKILL. */
#define SIGKILL_REPLACE		(SIGRTMIN + 1)

/*
 * unblock:
 * SIGILL		(4)
 * SIGBUS		(7)
 * SIGFPE		(8)
 * SIGKILL		(9)
 * SIGSEGV		(11)
 * SIGSTOP		(17)
 * SIG31(SIGUNUSED)	(31)
 * SIG32(SIGRTMIN)	(32)
 * SIGKILL_REPLACE	(33)
 * */
#define RECORD_PROCMASK_0 (0x3ffbfa37)
#define RECORD_PROCMASK_1 (0xfffffffe)
#define RECORD_PROCMASK	{RECORD_PROCMASK_0, RECORD_PROCMASK_1}

#define set_maskall_sigset(s)	do { \
	(s)->sig[0] = RECORD_PROCMASK_0;\
	(s)->sig[1] = RECORD_PROCMASK_1;\
} while(0)

#define unblock_sigkill_replace(s) do {\
	(s)->sig[1] &= 0xfffffffe;\
} while(0)

#define is_block_sigkill_replace(s) (((s)->sig[1]) & 0x1)

extern struct xmutex user_sigactions_mutex;
extern struct sigaction user_sigactions[];

extern sigset_t
block_signals(void);

extern void
restore_signals(sigset_t mask);

#endif

