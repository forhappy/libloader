.text
.globl _start

_start:
	movl $100000000, %ecx
	2: decl %ecx
	jz 1f
	jmp 2b
	1: movl $1, %eax
	int $0x80
