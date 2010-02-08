/* 
 * asm/debug.h
 * by WN @ Feb. 07, 2010
 */

#ifndef __ASM_DEBUG_H
#define __ASM_DEBUG_H

#define breakpoint()	do { asm volatile ("int3\n"); } while(0)

#endif

