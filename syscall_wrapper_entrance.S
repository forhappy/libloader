.section .text
.global _start

.type syscall_wrapper_entrace, @function
.global vdso_entrace
.type vdso_entrace, @object
.global vdso_ehdr
.type vdso_ehdr, @object

.section .data
vdso_entrace:
	.word 0
vdso_ehdr:
	.word 0
string_start:
	.asciz "syscall start\n"
string_end:
	.asciz "syscall end\n"
.previous	

syscall_wrapper_entrace:
	int $0x80
	ret


/*syscall_wrapper_entrace:*/
/* push args 
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	call syscall_wrapper
	popl %ebx	save eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	ret
*/

