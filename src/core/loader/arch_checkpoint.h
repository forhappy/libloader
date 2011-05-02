/* 
 * interp/arch_checkpoint.h
 * by WN @ Nov. 03, 2010
 *
 * architecture dependent checkpointing interface
 */

#ifndef ARCH_CHECKPOINT_H
#define ARCH_CHECKPOINT_H

#include <defs.h>
#include <loader/processor.h>

enum arch_ckpt_mark {
	ARCH_CKPT_SECT_UREGS_MARK = 0xffff,
	ARCH_CKPT_SECT_FXREGS_MARK = 0xfffe,
};

extern size_t
get_ckpt_cpuinfo(void * buffer, size_t sz,
		struct pusha_regs * regs, void * eip);

extern void
restore_ckpt_cpuinfo(void * buffer, size_t sz,
		struct pusha_regs * regs, void ** p_eip);

extern size_t
get_tls_info(void * buffer, size_t sz);

extern void
restore_tls_info(void * buffer, size_t sz);

#endif

// vim:ts=4:sw=4

