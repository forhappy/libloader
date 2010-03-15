/*
 * startup.S
 * by WN @ Mar. 16, 2010
 */

#include <asm_offsets.h>

.globl _start
 _start:
 	push $0
	pusha
	pushf
	/* xmain should be a hidden symbol */
	call xmain
	test %eax, %eax
	jne 1f
	popf
	popa
	jmp 2f
	1:
	popf
	popl %edi
	popl %esi
	popl %ebp
	addl $4, %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %eax
	movl -20(%esp), %esp
	/* begin running */
	2:
	popl %fs:OFFSET_CODE_CACHE_TARGET
	movl $0, %fs:OFFSET_CODE_CACHE_IF_TAKEN
	/* we reset esp after we pop the target. this is
	 because we are not in a real 'call'ed func */
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	/* switch stack! */
	movl %fs:OFFSET_STACK_TOP, %esp
	jmp *%fs:OFFSET_COMPILER_ENTRY

