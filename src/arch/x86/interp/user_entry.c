/* 
 * user_branch.c
 * by WN @ Apr. 26, 2010
 */

#include <common/defs.h>
#include <interp/user_entry.h>
#include <xasm/tls.h>
#include <xasm/syscall.h>
#include <xasm/utils.h>
#include <xasm/string.h>

#include <sys/mman.h>

/* patch the user entry: */
/* jmp user_branch */
/* NOTICS xxx xxx xxx xxx is difference of ptrs */
/* 0xe9 0xxx 0xxx 0xxx 0xxx */

#define PATCH_SZ (5)
static uint8_t saved_patch_bytes[PATCH_SZ];

static void * saved_user_entry = 0;
static void * saved_interp_entry = 0;

static void
mask_uent_pages(bool_t writable)
{
	uint32_t prot = PROT_READ | PROT_EXEC;
	if (writable)
		prot |= PROT_WRITE;
	void * start = ALIGN_DOWN_PTR(saved_user_entry, PAGE_SIZE);
	void * end = ALIGN_UP_PTR(saved_user_entry + PATCH_SZ, PAGE_SIZE);
	int err = INTERNAL_SYSCALL_int80(mprotect, 3, start, end - start, prot);
	assert(err == 0);
}

/* user_branch is defined in compiler.S, it calls do_user_branch */
extern void user_branch(void) ATTR_HIDDEN;
void
do_user_branch(struct pusha_regs regs)
{
	VERBOSE(LOADER, "in do_user_branch\n");
	struct thread_private_data * tpd = get_tpd();
	tpd->target = saved_user_entry;

	/* unpatch user code */
	mask_uent_pages(TRUE);
	memcpy(saved_user_entry, saved_patch_bytes, PATCH_SZ);
	mask_uent_pages(FALSE);

	enter_user_code(&regs, saved_user_entry);
	return;
}

void
first_branch(void * user_entry, void * interp_entry)
{
	TRACE(LOADER, "user_entry=%p, interp_entry=%p\n",
			user_entry, interp_entry);

	saved_user_entry = user_entry;
	saved_interp_entry = interp_entry;

	/* patch user code */
	mask_uent_pages(TRUE);
	memcpy(saved_patch_bytes, saved_user_entry, PATCH_SZ);

	uint8_t * ptr = user_entry;
	/* this is 'jmp user_branch' */
	ptr[0] = 0xe9;
	*((ptrdiff_t *)(&ptr[1])) = (void *)(user_branch) -
		(void *)(&ptr[PATCH_SZ]);
	TRACE(LOADER, "user_branch=%p\n", user_branch);

	mask_uent_pages(FALSE);
}

// vim:ts=4:sw=4

