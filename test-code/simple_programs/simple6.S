.data
_string:
	.asciz "Hello world!\n"

.text
.globl _start
_start:
	int3
	movl $4, %eax
	movl $1, %ebx
	movl $_string, %ecx
	movl $14, %edx
	int $0x80
	movl $1, %eax
	int $0x80


