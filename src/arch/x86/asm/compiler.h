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

extern void
compile_code_block(void);

#endif

// vim:ts=4:sw=4

