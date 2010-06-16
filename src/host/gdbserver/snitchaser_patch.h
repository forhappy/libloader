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

extern pid_t target_original_pid;
extern pid_t target_original_tid;
extern int target_tnr;
extern void * target_stack_base;

#endif

// vim:ts=4:sw=4

