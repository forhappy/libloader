/* 
 * tls.c
 * by WN @ Mar. 09, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/spinlock.h>
#include <asm/tls.h>
#include <asm/syscall.h>

/* 
 * thread_map is used to indicate the usage of ldt number. the
 * ldt slot is free if the corresponding bit is unset.
 * */
/* all tls related control work is protected by __tls_ctl_lock */

#define MAX_THREADS	(1 << 13)
static DEF_SPINLOCK_UNLOCKED(__tls_ctl_lock);
static uint32_t thread_map[MAX_THREADS / sizeof(uint32_t)];

void
init_tls(void)
{
	spin_lock(&__tls_ctl_lock);
	int pid, tid;
	pid = INTERNAL_SYSCALL_int80(getpid, 0);
	tid = INTERNAL_SYSCALL_int80(gettid, 0);
	DEBUG(TLS, "init TLS storage: pid=%d, tid=%d\n",
			tid, pid);
	spin_unlock(&__tls_ctl_lock);
}

// vim:ts=4:sw=4

