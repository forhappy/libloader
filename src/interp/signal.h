/* 
 * signal.h
 * by WN @ Apr. 06, 2010
 * all signal related operations
 */

#ifndef __INTERP_SIGNAL_H
#define __INTERP_SIGNAL_H

#include <xasm/types_helper.h>
#include <xasm/processor.h>

/* 
 * we shouldn't process SIGINT because snitchaser internal error generate
 * SIGINT and normal programs always omit it.
 */

/* 
 * we should have a special signal: if the process get locked and block all
 * signals, the only way to kill it is SIGKILL. However, if the program
 * receive SIGKILL, the logs won't get flushed. Therefore, we need a special
 * signal to replace SIGKILL. We should monitor each signal mask operations,
 * make them keep that signal unblocked. And when program finally receives
 * that signal, each thread should flush their logs, generate checkpoints
 * before their exit.
 *
 * SIGRTMAX is usable: it seems that no program relies on it.
 */

struct tls_signal {
	struct k_sigaction sigactions[K_NSIG+1];
	k_sigset_t sigmask;
};

extern void
init_tls_signal(struct tls_signal * tpd);

extern void
clear_tls_signal(struct tls_signal * tpd);

#endif

// vim:ts=4:sw=4

