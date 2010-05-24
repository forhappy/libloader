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
	pushl $0x0202
	popfl
	call do_real_branch
	popfl
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmpl *%fs:OFFSET_TARGET


/* user_branch is the patched '_start' code. The loader
 * needs to patch the '_start' code to direct control flow
 * to user_branch */
.globl user_branch
user_branch:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	call do_user_branch
	popfl
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	/* tpd->target should has been set by do_user_branch */
	jmpl *%fs:OFFSET_REAL_BRANCH

/* don't generate executable stack */
.section        .note.GNU-stack,"",@progbits

