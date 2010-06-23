/* 
 * x86 signal_numbers.h
 * by WN @ Jun. 23, 2010
 */

#ifndef __SIGNAL_NUMBERS_H
#define __SIGNAL_NUMBERS_H

#ifndef _NSIG
# define _NSIG	(64)
#endif

#ifndef SIGHUP
# define SIGHUP		 1
#endif
#ifndef SIGINT
# define SIGINT		 2
#endif
#ifndef SIGQUIT
# define SIGQUIT		 3
#endif
#ifndef SIGILL
# define SIGILL		 4
#endif
#ifndef SIGTRAP
# define SIGTRAP		 5
#endif
#ifndef SIGABRT
# define SIGABRT		 6
#endif
#ifndef SIGIOT
# define SIGIOT		 6
#endif
#ifndef SIGBUS
# define SIGBUS		 7
#endif
#ifndef SIGFPE
# define SIGFPE		 8
#endif
#ifndef SIGKILL
# define SIGKILL		 9
#endif
#ifndef SIGUSR1
# define SIGUSR1		10
#endif
#ifndef SIGSEGV
# define SIGSEGV		11
#endif
#ifndef SIGUSR2
# define SIGUSR2		12
#endif
#ifndef SIGPIPE
# define SIGPIPE		13
#endif
#ifndef SIGALRM
# define SIGALRM		14
#endif
#ifndef SIGTERM
# define SIGTERM		15
#endif
#ifndef SIGSTKFLT
# define SIGSTKFLT	16
#endif
#ifndef SIGCHLD
# define SIGCHLD		17
#endif
#ifndef SIGCONT
# define SIGCONT		18
#endif
#ifndef SIGSTOP
# define SIGSTOP		19
#endif
#ifndef SIGTSTP
# define SIGTSTP		20
#endif
#ifndef SIGTTIN
# define SIGTTIN		21
#endif
#ifndef SIGTTOU
# define SIGTTOU		22
#endif
#ifndef SIGURG
# define SIGURG		23
#endif
#ifndef SIGXCPU
# define SIGXCPU		24
#endif
#ifndef SIGXFSZ
# define SIGXFSZ		25
#endif
#ifndef SIGVTALRM
# define SIGVTALRM	26
#endif
#ifndef SIGPROF
# define SIGPROF		27
#endif
#ifndef SIGWINCH
# define SIGWINCH	28
#endif
#ifndef SIGIO
# define SIGIO		29
#endif
#ifndef SIGPOLL
# define SIGPOLL		SIGIO
#endif
/*
#ifndef SIGLOST
# define SIGLOST		29
#endif
*/
#ifndef SIGPWR
# define SIGPWR		30
#endif
#ifndef SIGSYS
# define SIGSYS		31
#endif
#ifndef SIGUNUSED
# define	SIGUNUSED	31
#endif

/* These should not be considered constants from userland.  */
#ifndef SIGRTMIN
# define SIGRTMIN	32
#endif
#ifndef SIGRTMAX
# define SIGRTMAX	_NSIG
#endif

#define GDBSERVER_NOTIFICATION	(SIGRTMAX - 1)


#endif

// vim:ts=4:sw=4

