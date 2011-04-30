/* 
 * checkpoint.h
 * by HP.Fu @ Apr. 18, 2011
 */

#ifndef __CHECKPOINT_H
#define __CHECKPOINT_H

#include <config.h>

#ifndef MAX_OUTPUT_FN_LEN
# define MAX_OUTPUT_FN_LEN	(128)
#endif

#ifdef __KERNEL__
# include <linux/time.h>
# include <linux/kernel.h>
# include <asm/page.h>

# define PROCMAP_GUARD		(1 * PAGE_SIZE)

#else
# include <sys/time.h>
# include <unistd.h>
#endif

#include <defs.h>
#include <processor.h>

#define CKPT_VERSION_STRING	"SNITCHASER-CKPT00"

enum ckpt_mark {
	CKPT_SECT_END_MARK	= 0,
	CKPT_SECT_EXEC_MARK,
	CKPT_SECT_ARGS_MARK,
	CKPT_SECT_ENVS_MARK,
	CKPT_SECT_CPU_MARK,
	CKPT_SECT_TLS_MARK,
	CKPT_SECT_MEMSEG_MARK,
	CKPT_SUBSECT_FN_MARK,
	CKPT_SUBSECT_FINFO_MARK,
	CKPT_SUBSECT_FDATA_MARK,
	NR_CKPT_SECT_MARKS,
};

struct checkpoint_head {
	char magic[sizeof(CKPT_VERSION_STRING)];
	uint32_t pid;
	uint32_t tid;
	int tnr;
	uintptr_t brk;
	uintptr_t stack_top;
};


struct map_file_info {
	uint32_t prot;
	uint64_t offset;
	/* how much space the file occupied */
	size_t space_sz;
	size_t ckpt_sz;
	uintptr_t start;
	uintptr_t end;
};

extern void
fork_checkpoint(struct pusha_regs * regs, void * pc,
		void * stack_top, struct timeval * ptv);

extern void
dead_checkpoint(struct pusha_regs * regs, void * pc,
		void * stack_top);


#endif

// vim:ts=4:sw=4

