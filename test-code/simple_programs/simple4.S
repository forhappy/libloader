.text
_func:
	addl $4, %esp
	ret

.globl _start
_start:
	pushl %ebp
	movl %esp, %ebp
	addl $-0x10, %esp

	pushl $_quit4
	movl $0x10000, %esi
	movl $_func, -0x40000(%ebp, %esi, 4)
	call *-0x40000(%ebp, %esi, 4)
_quit4:
	pushl $_quit3
	movl $_func, %eax
	call *%eax
_quit3:
	pushl $_quit2
	movl $_func, %ebx
	call *%ebx
_quit2:
	pushl $_quit1
	movl $_func, %ecx
	call *%ecx
_quit1:
	movl $1, %eax
	int $0x80

