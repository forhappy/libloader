/* 
 * user_entry.c
 * by WN @ Nov. 09, 2010
 */

#include <defs.h>
#include <debug.h>
#include <syscall.h>

#include <loader/proc.h>
#include <linux/mman.h>

#include <loader/snitchaser_tpd.h>
#include <loader/user_entry.h>
#include <loader/checkpoint.h>
#include <loader/startup_stack.h>
#include <loader/logger.h>

void
enter_user_entry(struct pusha_regs * regs, void * pc)
{
	assert(regs != NULL);
	struct thread_private_data * tpd = get_self_tpd();

	struct timeval tv;
	int err = sys_gettimeofday(&tv, NULL);
	assert(err == 0);
	fork_checkpoint(regs, pc, tpd->old_stack_top, &tv);

	init_self_logger(&tv, tpd->pid, tpd->tid);

#if 1 
	/* set resume_addr to pc then rebranch is turned off */
	tpd->resume_addr = pc;
#else
	/* begin instrumentation */
	tpd->resume_addr = (void *)(tpd->cc_stub.f_cc_compile);
#endif
}

void
patch_user_entry(void)
{
	void * uentry = *(STACK_AUXV_INFO(p_user_entry));
	DEBUG(LOADER, "patching user entry %p\n", uentry);

	void * mapdata = xalloc_pages(MAX_PROC_MAPS_FILE_SIZE, FALSE);
	assert(mapdata != NULL);

	struct proc_mem_handler_t h;
	read_self_procmap(&h, mapdata);
	struct proc_maps_entry_t r = find_in_map_by_addr(&h, uentry);

	TRACE(LOADER, "uentry in section (0x%lx - 0x%lx)\n", r.start, r.end);

	assert((uintptr_t)(uentry) >= r.start);
	assert((uintptr_t)(uentry) < r.end);
	if ((uintptr_t)(uentry) + PATCH_SZ >= r.end)
		FATAL(LOADER, "You have a very strange executable: "
				"start address %p is at the end of its memory section\n",
				uentry);

	void * reprotect_addr_start = (void*)(r.start);
	size_t reprotect_sz = (void*)(r.end) - reprotect_addr_start;
	uint32_t old_prot = r.prot;
	xdealloc_pages(mapdata, MAX_PROC_MAPS_FILE_SIZE);

	/* unprotect */
	int err = sys_mprotect(reprotect_addr_start, reprotect_sz,
			PROT_READ | PROT_WRITE);
	assert(err == 0);

	/* patching */
	arch_patch_user_entry(uentry, reprotect_addr_start,
			reprotect_sz, old_prot);

	/* reprotect */
	err = sys_mprotect(reprotect_addr_start, reprotect_sz, old_prot);
	assert(err == 0);
}
// vim:ts=4:sw=4

