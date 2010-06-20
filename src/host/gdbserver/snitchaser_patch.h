/* 
 * snitchaser_patch.h
 * by WN @ Jnu. 15, 2010
 *
 * snitchaser patch on gdbserver
 *
 */

#ifndef SNITCHASER_PATCH_H
#define SNITCHASER_PATCH_H

#include <common/defs.h>
#include <xasm/processor.h>
#include <stdint.h>
#include <unistd.h>	/* pid_t */
#include <sys/ptrace.h>

extern pid_t SN_target_original_pid;
extern pid_t SN_target_original_tid;
extern pid_t SN_target_pid;
extern int SN_target_tnr;
extern void * SN_target_stack_base;

void
SN_init(pid_t ori_pid, pid_t ori_tid,
		pid_t pid, int tnr, void * stack_base);

void
SN_reset_registers(void);

int
SN_ptrace_cont(enum __ptrace_request req, pid_t pid,
		uintptr_t addr, uintptr_t data);


#endif

// vim:ts=4:sw=4

