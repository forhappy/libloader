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

/* the basic checkpointing, no fork */
void
make_checkpoint(struct pusha_regs * regs);

#endif

// vim:ts=4:sw=4

