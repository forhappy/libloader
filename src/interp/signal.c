/* 
 * signal.c
 * by WN @ Apr. 06, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <xasm/syscall.h>
#include <xasm/types_helper.h>
#include <interp/signal.h>

void
init_tls_signal(struct tls_signal * ts)
{
	int err;
	TRACE(SIGNAL, "init tls signal\n");
	for (int i = 1; i <= K_NSIG; i++) {
		/* load the current sigmask */
		err = INTERNAL_SYSCALL_int80(rt_sigaction, 4,
				i, NULL, &(ts->sigactions[i]),
				sizeof(ts->sigmask));
		assert(err == 0);
	}
	err = INTERNAL_SYSCALL_int80(rt_sigprocmask, 4,
			SIG_BLOCK, NULL, &(ts->sigmask),
			sizeof(ts->sigmask));
	assert(err == 0);
}

void
clear_tls_signal(struct tls_signal * ts ATTR(unused))
{
	/* nothing todo */
	TRACE(SIGNAL, "clear tls signal\n");
}

// vim:ts=4:sw=4

