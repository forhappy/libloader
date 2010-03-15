/*
 * startup.S
 * by WN @ Mar. 16, 2010
 */

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
	ret
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
	ret

