/* 
 * arch_user_entry.c
 * by WN @ Nov. 08, 2010
 * modified by HP.Fu @ Apr. 30, 2011
 */


#include <linux/mman.h>
#include <defs.h>
#include <debug.h>
#include <loader/proc.h>
#include <syscall.h>
#include <loader/processor.h>
#include <loader/mm.h>
#include <loader/user_entry.h>
#include <loader/startup_stack.h>


static void * reprotect_addr_start = NULL;
static size_t reprotect_sz = 0;
static uint32_t old_prot = 0;
static void * patch_begin = NULL;
static uint8_t saved_patch_bytes[PATCH_SZ];

/* patch the user entry: */
/* jmp user_branch */
/* NOTICE: xxx xxx xxx xxx is difference of ptrs */
/* 0xe9 0xxx 0xxx 0xxx 0xxx */

static void
do_patch(void * inst, void * target)
{
	uint8_t * ptr = inst;
	ptrdiff_t diff = target - (inst + PATCH_SZ);
	ptr[0] = 0xe9;
	*(ptrdiff_t*)(&ptr[1]) = diff;
}

static void
unpatch_user_entry(void)
{

	VERBOSE(LOADER, "comes to unpatch user entry\n");

	assert(patch_begin != NULL);
	assert(reprotect_addr_start != NULL);

	/* unprotect */
	int err = sys_mprotect(reprotect_addr_start, reprotect_sz,
			PROT_READ | PROT_WRITE);
	assert(err == 0);

	/* unpatching */
	memcpy(patch_begin, saved_patch_bytes, PATCH_SZ);

	/* reprotect */
	err = sys_mprotect(reprotect_addr_start, reprotect_sz, old_prot);
	assert(err == 0);
}

void
arch_patch_user_entry(void * uentry, void * rep_start,
		        size_t rep_sz, uint32_t prot)
{

	VERBOSE(LOADER, "comes to arch_patch_user_entry\n");
	patch_begin = uentry;
	memcpy(saved_patch_bytes, patch_begin, PATCH_SZ);
	reprotect_addr_start = rep_start;
	reprotect_sz = rep_sz;
	old_prot = prot;
	do_patch(patch_begin, arch_user_entry_e);
}

void
arch_user_entry(struct pusha_regs * regs)
{
	VERBOSE(LOADER, "comes to user entry\n");
	/* unpatching */
	unpatch_user_entry();
	enter_user_entry(regs, patch_begin);
}

// vim:ts=4:sw=4

