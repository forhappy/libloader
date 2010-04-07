/* 
 * checkpoint.h
 * by WN @ Apr. 06, 2010
 */

#ifndef __CHECKPOINT_H
#define __CHECKPOINT_H

#include <config.h>
#include <stdint.h>
#include <interp/logger.h>
#include <xasm/types_helper.h>
#include <xasm/processor.h>

#define CKPT_MAGIC	"SNITCHASER CKPT"
#define CKPT_MAGIC_SZ	(16)

#define MAX_PROC_MAPS_FILE_SZ	(1024*1024)

struct checkpoint_head {
	char magic[CKPT_MAGIC_SZ];
	uint32_t brk;
	uint32_t pid;
	uint32_t tid;
	int tnr;
	struct user_desc thread_area[GDT_ENTRY_TLS_ENTRIES];
	/* signal 0 is no use */
	struct k_sigaction sigactions[K_NSIG+1];
	k_sigset_t sigmask;
	struct user_regs_struct regs;
	struct i387_fxsave_struct fpustate;
};

struct mem_region {
	uintptr_t start;
	uintptr_t end;
	uint32_t prot;
	uint32_t offset;
	int fn_sz;
	char fn[];
};
#define MEM_REGIONS_END_MARK	(0xfefefefe)

/* the basic checkpointing, no fork */
/* the fork based checkpoint should unmap all logger pages
 * and codecache pages, if not, 
 * size of checkpoint will become very large */
void
make_checkpoint(struct pusha_regs * regs, void * eip);

void
fork_make_checkpoint(struct pusha_regs * regs, void * eip);

#endif

// vim:ts=4:sw=4

