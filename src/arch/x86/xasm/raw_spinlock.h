/* 
 * spinlock.h
 * by WN @ Mar. 08, 2010
 */


#ifndef __X86_SPINLOCK_H
#define __X86_SPINLOCK_H

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>

struct raw_spinlock_t {
	volatile unsigned int lock;
};

#define RAW_SPINLOCK_UNLOCKED	{ 0 }
#define INIT_RAW_SPINLOCK_UNLOCKED(l)	do {(l)->lock = 0;} while(0)

static __AI int
raw_spin_trylock(struct raw_spinlock_t * lock)
{
	int ret = 1;
	asm volatile (
			"xchgl %0, %1\n"
			: "+r" (ret), "+m" (lock->lock)
			:
			: "memory"
			);
	return (ret == 0);
}

static __AI void
raw_spin_lock(struct raw_spinlock_t * lock)
{
	register unsigned long tmp = 1;
	asm volatile (
			"1: xchgl %1, %0\n"
			"test %1, %1\n"
			"jz 2f\n"
			"pause\n"
			"jmp 1b\n"
			"2:\n"
			: "+m" (lock->lock), "=r" (tmp)
			: "1" (tmp)
			: "memory", "cc"
			);
}


/* return value is the original valued */
static __AI int
raw_spin_unlock(struct raw_spinlock_t * lock)
{
	int ret = 0;
	asm volatile (
			"xchgl %0, %1\n"
			: "+r" (ret), "+m" (lock->lock)
			:
			: "memory"
			);
	return ret;
}

static __AI int
raw_spin_is_locked_by_value(int val)
{
	return (val != 0);
}

static __AI int
raw_spin_is_locked(struct raw_spinlock_t * lock)
{
	return (lock->lock!= 0);
}

#endif

// vim:ts=4:sw=4

