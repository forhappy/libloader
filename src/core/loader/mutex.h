/* 
 * mutex.h
 * by WN @ Oct. 22, 2010
 */

#ifndef __X86_MUTEX_H
#define __X86_MUTEX_H

#include <asm/atomic.h>
#include <common/defs.h>
#include <xasm/signal.h>

/* to avoid namespace conflict: kernel include files define 'struct mutex' */
struct xmutex {
	atomic_t val;
};

#define DEFINE_UNLOCKED_XMUTEX(x)	struct xmutex x = { .val = { .counter = 0 } }

static inline bool_t
xmutex_is_locked(struct xmutex * x)
{
	return (atomic_read(&x->val) != 0);
}

static inline void
xmutex_init(struct xmutex * lock)
{
	atomic_set(&lock->val, 0);
}


extern sigset_t
xmutex_lock(struct xmutex * lock);

extern void
xmutex_unlock(struct xmutex * lock, sigset_t mask);

#endif

// vim:ts=4:sw=4

