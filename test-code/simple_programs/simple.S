.text
.globl _start

_start:
	movl $900000000, %ecx
	1:
	decl %ecx
	jnz 1b
	movl $1, %eax
	int $0x80

