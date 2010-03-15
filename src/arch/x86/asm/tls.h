/* 
 * tls.h
 * by WN @ Mar. 09, 2010
 */

#ifndef __X86_TLS_H
#define __X86_TLS_H

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>
#include <asm/syscall.h>
#include <interp/dict.h>
#include <interp/code_cache.h>

/* thread thread stack: from 0x3000, each for 3 pages(8k + 4k)
 * the upper 1 page is protected to prevent stack overflow
 * */
/* x86 have at most 8192 ldt entry, so at most 8192 tls.
 * each thread's tls section is predefined:
 * each thread has 2 pages for stack, the thread self's info
 * is located at the bottom of the stack.
 * if we have 8192 (0x2000) threads, the tls stack is from 0x3000 to 0x6003000;
 * normal program always put its ode section at 0x8048000, so
 * the stack never clobber code section.
 */

/* 
 * What's in the tls data section?
 */
struct thread_private_data {
	void * ret_address;
	struct tls_code_cache_t code_cache;
	/* still need: head address of dict; head address of code cache */
	/* tnr is thread identifier using in snitchaser */
	int tid, pid;
	unsigned int tnr;
	/* we also need some code cache stuff */
	/* when enter snitchaser's code, stack should be set to it */
	struct thread_private_data * stack_top;	
	/* access: *fs:(0x3000 - 4); stores: the base (lowest)
	 * address of the TLS section */
	void * tls_base;	
};

#define TLS_STACK_SIZE		(0x3000)
#define MAX_TLS_ADDR_FS		(TLS_STACK_SIZE)
#define TPD_ADDR_FS			(MAX_TLS_ADDR_FS-sizeof(struct thread_private_data))
#define TNR_TO_STACK(n)		((void*)(((n) + 1) * TLS_STACK_SIZE))
#define STACK_TO_TNR(a)		(((unsigned int)(a)) / TLS_STACK_SIZE - 1)
#define GUARDER_START		(0)
#define GUARDER_LENGTH		(0x1000)


/* init_tls ans clear_tls should be called
 * with thread original stack, not the snitchaser's
 * TLS stack! */
extern void init_tls(void);
extern void clear_tls(void);

static __AI void *
get_tls_base(void)
{
	void * r;
	asm volatile (
			"movl %%fs:%c[b_addr], %[o]\n"
			: [o] "=r" (r)
			: [b_addr] "i" (TPD_ADDR_FS +
				offsetof(struct thread_private_data, tls_base))
			);
	return r;
}

static __AI struct thread_private_data *
get_tpd(void)
{
	void * r;
	asm volatile (
			"movl %%fs:%c[tpd_addr], %[o]\n"
			: [o] "=r" (r)
			: [tpd_addr] "i" (TPD_ADDR_FS +
				offsetof(struct thread_private_data, stack_top))
			);
	return r;
}

static __AI uint32_t
get_esp(void)
{
	uint32_t r;
	asm volatile ("movl %%esp, %[r]\n"
			: [r] "=rm" (r));
	return r;
}

#endif

// vim:ts=4:sw=4

