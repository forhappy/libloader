/* 
 * x86/asm/atomic.h
 * by WN @ Mar. 09, 2010
 */

#ifndef __X86_ATOMIC_H
#define __X86_ATOMIC_H

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>

typedef struct atomic_t {
	volatile int counter;
} atomic_t;


static __AI void
atomic_inc(atomic_t *v)
{
	asm volatile("lock incl %0"
		     : "+m" (v->counter));
}

static __AI void
atomic_dec(atomic_t *v)
{
	asm volatile("lock decl %0"
		     : "+m" (v->counter));
}

static __AI void
atomic_add(int i, atomic_t *v)
{
	asm volatile("lock addl %1,%0"
		     : "+m" (v->counter)
		     : "ir" (i));
}

static inline void atomic_sub(int i, atomic_t *v)
{
	asm volatile("lock subl %1,%0"
		     : "+m" (v->counter)
		     : "ir" (i));
}

static __AI void
atomic_set(atomic_t *v, int i)
{
	v->counter = i;
}

static __AI int
atomic_read(const atomic_t *v)
{
	return v->counter;
}


static __AI int
atomic_add_return(int i, atomic_t *v)
{
	int __i;
	__i = i;
	asm volatile("lock xaddl %0, %1"
		     : "+r" (i), "+m" (v->counter)
		     : : "memory");
	return i + __i;
}

static __AI int
atomic_sub_return(int i, atomic_t *v)
{
	return atomic_add_return(-i, v);
}

static __AI int
atomic_cmpxchg(atomic_t * v, unsigned long old, unsigned long new)
{
	unsigned long prev;
	asm volatile("lock cmpxchgl %1, %2\n"
			: "=a" (prev)
			: "r" (new), "m" (v->counter), "0" (old)
			: "memory");
	return prev;
}

static __AI int
atomic_xchg(atomic_t * v, unsigned long x)
{
	asm volatile("xchgl %0, %1\n"
			: "=r" (x)
			: "m" (v->counter), "0" (x)
			: "memory");
	return x;
}


#endif

// vim:ts=4:sw=4

