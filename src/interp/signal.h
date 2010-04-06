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

