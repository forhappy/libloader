/* 
 * arch_replayer.h
 * by WN @ Jun. 18, 2010
 */

#ifndef X86_ARCH_REPLAYER_H
#define X86_ARCH_REPLAYER_H

#include <sys/user.h>
#include <xasm/processor.h>

#include <unistd.h>

void
ptrace_get_regset(pid_t pid, struct user_regs_struct * urs);

void
ptrace_set_regset(pid_t pid, struct user_regs_struct * urs);

uintptr_t
ptrace_get_eip(pid_t pid);

void
ptrace_set_eip(pid_t pid, uintptr_t eip);

/* if eip == NULL, don't reset eip */
void
arch_restore_registers(pid_t pid, struct pusha_regs * regs, void * eip);

#endif

// vim:ts=4:sw=4

