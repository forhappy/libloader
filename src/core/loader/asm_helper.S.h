/*
 * asm_helper.S.h
 * provides some macros for asm programming
 */

#ifndef __ASM_HELPER_S_H
#define __ASM_HELPER_S_H

#include <interp/rebranch_asm_offset.h>

#define ENTER_ENTRY	\
		movl %esp, %fs:OFFSET_TPD_OLD_STACK_TOP; \
		movl %fs:OFFSET_TPD_STACK_TOP, %esp; \
		pusha; \
		pushf; \
		pushl $0x0202; \
		popfl; \
		push %esp

/* 'addl' inst may modify eflags, but we restore it immediately */
#define PREPARE_EXIT \
		addl $4, %esp; \
		popf; \
		popa; \
		movl %fs:OFFSET_TPD_OLD_STACK_TOP, %esp

#define EXIT_TO(func) \
	PREPARE_EXIT; \
	jmpl *%fs:func

#endif

// vim:ts=4:sw=4

