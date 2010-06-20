/* 
 * asm/x86/compiler.h
 * by WN @ Mar. 22, 2010
 */

#ifndef __X86_COMPILER_H
#define __X86_COMPILER_H

#include <config.h>
#include <common/defs.h>

extern ATTR_HIDDEN void *
scan_insts(void * stream);

extern ATTR_HIDDEN void
real_branch(void);

extern void
replay_patch_block(void);

extern void
replay_unpatch_block(void);


#endif

