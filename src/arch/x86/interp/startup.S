/*
 * startup.S
 * by WN @ Mar. 16, 2010
 */

#include <asm_offsets.h>
#include <config.h>

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
	popl %fs:OFFSET_TARGET
	jmpl *%fs:OFFSET_REAL_BRANCH

.globl _xstart
_xstart:
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
	2:	/* ret to its original entry */
	ret

.globl DEBUG_ENTRY
DEBUG_ENTRY:
	movl $1, %eax
	int $0x80


