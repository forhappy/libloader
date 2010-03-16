/*
 * compiler.S
 * by WN @ Mar. 16, 2010
 */

#include <asm_offsets.h>

.text
.globl logger_entry
logger_entry:
	/* switch stack! */
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf

	call heavy_logger_entry

	popf
	popa
	int3
	/* switch stack back */
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmp *%fs:OFFSET_EXIT_ADDR

.globl ud_logger_entry
ud_logger_entry:
	jmp logger_entry

.globl ui_logger_entry
ui_logger_entry:
	jmp logger_entry

.globl cd_logger_entry
cd_logger_entry:
	jmp logger_entry

.globl ci_logger_entry
ci_logger_entry:
	jmp logger_entry

// vim:ts=4:sw=4

