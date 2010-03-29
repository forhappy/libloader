/*
 * compiler.S
 * by WN @ Mar. 22, 2010
 */

#include <asm_offsets.h>

.text
.globl real_branch
real_branch:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0
	popf
	call do_real_branch
	popf
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmpl *%fs:OFFSET_TARGET

