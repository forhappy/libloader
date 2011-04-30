/* 
 * tls.c
 * by WN @ Oct. 25, 2010
 */

#include <linux/mman.h>
#include <asm/ldt.h>

#include <defs.h>
#include <bithacks.h>
#include <debug.h>
#include <list.h>
#include <mutex.h>
#include <syscall.h>
#include <asm_syscall.h>
#include <loader/tls.h>
#include <loader/mm.h>


#define READ_LDT		(0)
#define WRITE_LDT_OLD		(1)
#define READ_DEFAULT_LDT	(2)
#define WRITE_LDT			(0x11)

DEFINE_UNLOCKED_XMUTEX(tls_lock);
LIST_HEAD(tls_list);

#define MAX_THREADS	(LDT_ENTRIES)
#define SZ_MAP	(MAX_THREADS / sizeof(uint32_t))
static uint32_t thread_map[SZ_MAP];

static int
find_set_free_slot(void)
{
	assert(xmutex_is_locked(&tls_lock));
	for (int i = 0; i < (int)SZ_MAP; i++) {
		if (thread_map[i] != 0xffffffff) {
			int n = i * 32 + last_0_pos(thread_map[i]);
			thread_map[i] = set_last_0(thread_map[i]);
			return n;
		}
	}
	FATAL(TLS, "TLS slots is full\n");
}

static void
clear_slot(unsigned int tnr)
{
	int nr = tnr / 32;
	int n = tnr % 32;
	thread_map[nr] = unset_bit_n(thread_map[nr], n);
}


static void
write_ldt(struct user_desc * desc)
{
	int err;
	err = INTERNAL_SYSCALL_int80(modify_ldt, 3,
			WRITE_LDT, desc, sizeof(*desc));
	assert(err == 0);
}


void
setup_ldt(struct tls_desc * td, int nr)
{
	assert(td != NULL);

	struct user_desc desc;
	desc.entry_number = nr;
	desc.base_addr = (unsigned long)(td->start_addr);
	desc.limit = NR_TLS_PAGES;
	desc.seg_32bit = 1;
	desc.contents = 0;
	desc.read_exec_only = 0;
	desc.limit_in_pages = 1;
	desc.seg_not_present = 0;
	desc.useable = 1;
	write_ldt(&desc);

	/* segregister: 16bits
	 * 
	 * X X X X X X X X X X X X X X X X
	 *                           ^ ^ ^
	 *                           | |-|
	 *        <--- Index ---    TI RPL
	 * 3 is 'user'
	 * */
	td->nr = nr;
	td->fs_val = nr * 8 + 4 + 3;
}

static void
clear_ldt(struct tls_desc * td)
{
	assert(td != NULL);
	int nr = td->nr;
	assert(nr * 8 + 4 + 3 == td->fs_val);

	struct user_desc desc;
	desc.entry_number = nr;
	desc.base_addr = 0;
	desc.limit = 0;
	desc.contents = 0;
	desc.read_exec_only = 1;
	desc.seg_32bit = 0;
	desc.limit_in_pages = 0;
	desc.seg_not_present = 1;
	desc.useable = 0;
	write_ldt(&desc);
}


struct tls_desc *
alloc_tls_area(void)
{
	DEBUG(TLS, "allocating TLS area\n");
	sigset_t saved_sigset = xmutex_lock(&tls_lock);
	TRACE(TLS, "lock gained\n");

	void * pages = xalloc_pages(TLS_AREA_SIZE, FALSE);
	TRACE(TLS, "pages (%lu bytes) allocated: %p\n",
			TLS_AREA_SIZE, pages);

	/* reprotect low page */
	sys_mprotect(pages, PAGE_SIZE, PROT_NONE);

	struct tls_desc * td = get_tls_desc(pages);

	td->start_addr = pages;
	td->size = TLS_AREA_SIZE;

	/* setup tls slot */
	setup_ldt(td, find_set_free_slot());
	TRACE(TLS, "tls_desc at %p, slot nr: %d\n", td, td->nr);
	
	list_add(&td->list, &tls_list);

	xmutex_unlock(&tls_lock, saved_sigset);
	DEBUG(TLS, "get TLS area, nr: %d, pages: %p, tls_desc: %p, size: %dkB\n",
			td->nr, pages, td, td->size / 1024);

	return td;
}

void
dealloc_tls_area_nolock(struct tls_desc * td)
{
	assert(td != NULL);
	assert(get_tls_desc(td->start_addr) == td);
	assert(td->size == TLS_AREA_SIZE);

	list_del(&td->list);
	clear_ldt(td);

	/* clear slot */
	clear_slot(td->nr);

	/* clear pages */
	xdealloc_pages(td->start_addr, td->size);
}

void
dealloc_tls_area(struct tls_desc * td)
{
	sigset_t saved_sigset = xmutex_lock(&tls_lock);
	dealloc_tls_area_nolock(td);
	xmutex_unlock(&tls_lock, saved_sigset);
}

// vim:ts=4:sw=4

