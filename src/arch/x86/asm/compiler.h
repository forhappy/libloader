/* 
 * x86/asm/compiler.h
 * by WN @ Mar. 15, 2010
 */

#ifndef X86_ASM_COMPILER_H
#define X86_ASM_COMPILER_H

#include <stdint.h>
#include <interp/code_cache.h>

/* 
 * implemented in scan_opcode.c, used in compile_code_block
 * input is the entry of the instruction stream,
 * return value is its exit instruction
 */
extern void *
scan_insts(void * stream);

/* stream is the real address of the code block,
 * branch_inst is its exit,
 * return value is the already-in-cache
 * code_block_t */
extern struct code_block_t *
compile_code_block(void * stream);

/* if the target address of an code block is
 * fixed (this is a unconditional, direct, branch),
 * then recompile it. 
 *
 * !!NOTE!!
 * after recompilation, the size of the block MUST shrink.
 * This is not a very strick problem. 
 * */
extern void
recompile_ud_code_block(void);

/* this is the MOST important entry: each time
 * a new code block is hit, control is redirected here */
/* no param, all information is set using TLS code cache section. */
extern void
compiler_entry(void);

#endif

// vim:ts=4:sw=4

