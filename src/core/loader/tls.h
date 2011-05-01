/* 
 * x86/interp/tls.h
 * by WN @ Oct. 24, 2010
 */

#ifndef _LOADER_TLS_H
#define _LOADER_TLS_H

#include <config.h>
#include <defs.h>
#include <list.h>
#include <mutex.h>
#include <asm/page.h>
#include <linux/stddef.h>

#include <loader/bigbuffer.h>
#define NR_TLS_PAGES	((BIGBUFFER_PAGES) + (BIGBUFFER_EXTRA_PAGES) + 4)
#define TLS_AREA_SIZE	(NR_TLS_PAGES * PAGE_SIZE)

typedef uint16_t tls_asm_slot_t;

struct tls_desc {
	tls_asm_slot_t fs_val;
	int nr;
	void * start_addr;
	size_t size;
	struct list_head list;
};

extern struct xmutex tls_lock;
extern struct list_head tls_list;

/* the bottom of a tls area is the tls desc */

#define get_tls_topmost_addr(a)	((void*)((void*)(a) + TLS_AREA_SIZE\
						- sizeof(struct tls_desc)))
#define get_tls_desc(a)	((struct tls_desc *)(get_tls_topmost_addr(a)))

extern struct tls_desc *
alloc_tls_area(void);

extern void
dealloc_tls_area(struct tls_desc * desc);

/* for checkpoint use, deallocate tls area without locking */
extern void
dealloc_tls_area_nolock(struct tls_desc * desc);

static inline void
active_tls(struct tls_desc * desc)
{
	asm volatile ("movw %w0, %%fs\n"
					:
					: "R" (desc->fs_val)
					: "memory");
}

static inline void
deactive_tls(void)
{
	asm volatile ("movw %w0, %%fs\n"
					:
					: "R" (0)
					: "memory");
}

static inline struct tls_desc *
get_self_tls_desc(void)
{
	void * tls_addr;
	asm volatile (
			"movl %%fs:%c[ta], %[o]\n"
			: [o] "=r" (tls_addr)
			: [ta] "i" (TLS_AREA_SIZE - sizeof(struct tls_desc) +
				offsetof(struct tls_desc, start_addr))
			);
	return get_tls_desc(tls_addr);
}

extern void
setup_ldt(struct tls_desc * td, int nr);

#endif

// vim:ts=4:sw=4

