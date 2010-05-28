/* 
 * syscall_table.h
 * by WN @ May. 27, 2010
 */

#ifndef SYSCALL_TABLE_H
#define SYSCALL_TABLE_H

#include <xasm/processor.h>

struct syscall_table_entry {
	int (*pre_handler)(struct pusha_regs * regs);
	int (*post_handler)(struct pusha_regs * regs);
	int (*replay_handler)(struct pusha_regs * regs);
};

extern struct syscall_table_entry syscall_table[];

#endif

// vim:ts=4:sw=4

