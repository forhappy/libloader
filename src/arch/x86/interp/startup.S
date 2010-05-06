/*
 * startup.S
 * by WN @ Mar. 16, 2010
 */

#include <asm_offsets.h>
#include <config.h>

.globl _start
_start:
	/* the entry of interp */
 	push $0
	/* the user entry */
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
	call *%fs:OFFSET_FIRST_BRANCH
	addl $4, %esp
	ret

.globl _xstart
_xstart:
 	push $0
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
	addl $4, %esp
	ret

.globl REPLAYER_ENTRY
REPLAYER_ENTRY:
	call replayer_main
	movl $1, %eax
	int $0x80

/* don't generate executable stack */
.section        .note.GNU-stack,"",@progbits

