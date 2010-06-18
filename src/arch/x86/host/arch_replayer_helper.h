/* 
 * arch_replayer.h
 * by WN @ Jun. 18, 2010
 */

#ifndef X86_ARCH_REPLAYER_H
#define X86_ARCH_REPLAYER_H

#include <xasm/processor.h>

#include <unistd.h>

/* if eip == NULL, don't reset eip */
void
arch_restore_registers(pid_t pid, struct pusha_regs * regs, void * eip);

#endif

// vim:ts=4:sw=4

