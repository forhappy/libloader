/* 
 * kutils.h
 * by WN @ Mar. 22, 2010
 * utils which related to kernel stuff
 */

#ifndef __X86_ASM_KUTILS_H
#define __X86_ASM_KUTILS_H

#include <config.h>

extern void
block_signals(void * save_sigmask, int * block_level);

extern void
restore_signals(void * save_sigmask, int * block_level);

#ifdef RELAX_SIGNAL
# define BLOCK_SIGNALS(a) do {  } while(0)
# define UNBLOCK_SIGNALS(a) do {  } while(0)
#else
# define BLOCK_SIGNALS(tpd) do {block_signals((tpd)->sigmask, &(tpd)->sig_block_level);} while(0)
# define UNBLOCK_SIGNALS(tpd) do {restore_signals((tpd)->sigmask, &(tpd)->sig_block_level);} while(0)
#endif

#endif

