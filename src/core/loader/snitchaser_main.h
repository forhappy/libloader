/*
 * =====================================================================================
 *
 *       Filename:  snitchaser_main.h
 *
 *    Description:  snitchaser main entry
 *
 *        Version:  1.0
 *        Created:  04/13/2011 09:55:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HP.Fu
 *        Company:  ICT
 *
 * =====================================================================================
 */
#ifndef __SNITCHASER_MAIN_H
#define __SNITCHASER_MAIN_H


#include <config.h>
#include <defs.h>
#include <debug.h>
#include <syscall.h>

#include <linux/kernel.h>
#include <linux/personality.h>

#include <loader/proc.h>
#include <loader/processor.h>
struct snitchaser_startup_stack {
	volatile struct pusha_regs saved_regs;
	void * volatile retaddr;
};
extern int logger_fd;
#endif
