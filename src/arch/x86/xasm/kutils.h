/* 
 * kutils.h
 * by WN @ Mar. 22, 2010
 * utils which related to kernel stuff
 */

#ifndef __X86_ASM_KUTILS_H
#define __X86_ASM_KUTILS_H

#include <config.h>

extern void
block_signals(void);

extern void
restore_signals(void);

#ifdef RELAX_SIGNAL
# define block_signals do {  } while(0)
# define restore_signals do {  } while(0)
#endif

#endif

