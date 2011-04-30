/* 
 * x86/interp/mutex.c
 * by WN @ Oct. 24, 2010
 */

#include <asm/atomic.h>
#include <xasm/mutex.h>
#include <xasm/futex.h>
#include <xasm/signal.h>
#include <xasm/syscall.h>

#include <common/debug.h>

sigset_t
xmutex_lock(struct xmutex * lock)
{
	TRACE(MUTEX, "thread %d:%d is locking %p\n",
			sys_getpid(), sys_gettid(), lock);
	int c;
	sigset_t sigset = block_signals();
	if ((c = atomic_cmpxchg(&lock->val, 0, 1)) != 0) {
		do {
			if (c == 2 || atomic_cmpxchg(&lock->val, 1, 2) != 0)
				futex_wait(&lock->val, 2);
		} while ((c = atomic_cmpxchg(&lock->val, 0, 2)) != 0);
	}
	TRACE(MUTEX, "thread %d:%d gained lock %p\n",
			sys_getpid(), sys_gettid(), lock);
	return sigset;
}

void
xmutex_unlock(struct xmutex * lock, sigset_t sigset)
{
	TRACE(MUTEX, "thread %d:%d is unlocking %p\n",
			sys_getpid(), sys_gettid(), lock);
	/* atomic_dec(&lock->val) != 1 */
	if (!atomic_dec_and_test(&lock->val)) {
		atomic_set(&lock->val, 0);
		futex_wake(&lock->val, 1);
	}
	restore_signals(sigset);
	TRACE(MUTEX, "thread %d:%d unlocked %p\n",
			sys_getpid(), sys_gettid(), lock);
}

// vim:ts=4:sw=4

