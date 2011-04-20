/* 
 * debug.h
 * by WN @ Nov. 08, 2009
 * modified by HP.Fu @ Mar. 20, 2011
 */

#ifndef __DEBUG_H
#define __DEBUG_H
#include <defs.h>

#define DBG_OUTPUT_FILENO      (1020)

inline static void
breakpoint(void)
{
	asm volatile ("int3\n");
}

/* don't use original assert: printf is not ok */
#define assert_noprint(expr)	\
  ((expr)						\
   ? (0)						\
   : breakpoint())

#endif

// vim:ts=4:sw=4

