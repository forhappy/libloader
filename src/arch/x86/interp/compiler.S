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
	/* NOTICE: %fs:OFFSET_OLD_STACK_TOP should have been adjusted by
	 * do_user_branch:
	 * minus by 4 and push target address
	 */
	ret
#if 0
	popl %fs:OFFSET_TARGET
	jmpl *%fs:OFFSET_REAL_BRANCH
#endif

