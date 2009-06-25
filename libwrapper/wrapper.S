	.file	"wrapper.c"
	.data
	.type	help_string, @object
	.size	help_string, 21
help_string:
	.string	"this is help string\n"
	.L__X'%ebx = 1
	.L__X'%ecx = 2
	.L__X'%edx = 2
	.L__X'%eax = 3
	.L__X'%esi = 3
	.L__X'%edi = 3
	.L__X'%ebp = 3
	.L__X'%esp = 3
	.macro bpushl name reg
	.if 1 - \name
	.if 2 - \name
	error
	.else
	xchgl \reg, %ebx
	.endif
	.endif
	.endm
	.macro bpopl name reg
	.if 1 - \name
	.if 2 - \name
	error
	.else
	xchgl \reg, %ebx
	.endif
	.endif
	.endm
	
	.text
.globl show_help
	.hidden	show_help
	.type	show_help, @function
show_help:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$48, %esp
	call	__i686.get_pc_thunk.bx
	addl	$_GLOBAL_OFFSET_TABLE_, %ebx
	movl	$1, %edi
	leal	help_string@GOTOFF(%ebx), %ecx
	movl	$21, %edx
	xchgl %ebx, %edi
	movl $4, %eax
int $0x80
xchgl %edi, %ebx
	
	movl $1, %eax
int $0x80

	movl	$16, %ecx
	bpushl .L__X'%ecx, %ecx
	movl $1, %eax
int $0x80
bpopl .L__X'%ecx, %ecx
	
	movl	$16, %edx
	movl	$17, %ecx
	bpushl .L__X'%edx, %edx
	movl $1, %eax
int $0x80
bpopl .L__X'%edx, %edx
	
	movl	$16, %edi
	movl	$17, %ecx
	movl	$18, %edx
	xchgl %ebx, %edi
	movl $1, %eax
int $0x80
xchgl %edi, %ebx
	
	movl	$16, %edi
	movl	$17, %ecx
	movl	$18, %edx
	movl	$19, %esi
	xchgl %ebx, %edi
	movl $1, %eax
int $0x80
xchgl %edi, %ebx
	
	movl	$16, %eax
	movl	$17, %ecx
	movl	$18, %edx
	movl	$19, %esi
	movl	$20, %edi
	movl	%eax, -36(%ebp)
	movl	-36(%ebp), %eax
	movl %ebx, %eax
	movl %eax, %ebx
	movl $1, %eax
int $0x80
movl %eax, %ebx
	movl	$21, -24(%ebp)
	movl	$16, %eax
	movl	$17, %ecx
	movl	$18, %edx
	movl	$19, %esi
	movl	$20, %edi
	movl	%eax, -32(%ebp)
	movl	-32(%ebp), %eax
	movl %ebx, -20(%ebp)
movl %eax, %ebx
movl $1, %eax
xchgl -24(%ebp), %ebp
int 0x80
xchgl %ebp, -24(%ebp)
movl -20(%ebp), %ebx

	addl	$48, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.size	show_help, .-show_help
	.section	.rodata
.LC0:
	.string	"XX:0x%x:XX\n"
	.text
.globl injector_entry
	.hidden	injector_entry
	.type	injector_entry, @function
injector_entry:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	subl	$96, %esp
	call	__i686.get_pc_thunk.bx
	addl	$_GLOBAL_OFFSET_TABLE_, %ebx
	movl	$4, %eax
	movl	$1, %esi
	leal	help_string@GOTOFF(%ebx), %ecx
	movl	$21, %edx
		xchgl %ebx, %esi
	int $0x80
	xchgl %ebx, %esi

	movl	$4660, 12(%esp)
	leal	.LC0@GOTOFF(%ebx), %eax
	movl	%eax, 8(%esp)
	movl	$64, 4(%esp)
	leal	-76(%ebp), %eax
	movl	%eax, (%esp)
	call	snprintf@PLT
	movl	%eax, -12(%ebp)
	movl	$4, %eax
	movl	$1, %esi
	leal	-76(%ebp), %ecx
	movl	-12(%ebp), %edx
		xchgl %ebx, %esi
	call __vsyscall
	xchgl %ebx, %esi

	movl	$1, %eax
		int $0x80

	addl	$96, %esp
	popl	%ebx
	popl	%esi
	popl	%ebp
	ret
	.size	injector_entry, .-injector_entry
	.hidden	old_ventry
	.comm	old_ventry,4,4
	.hidden	old_vhdr
	.comm	old_vhdr,4,4
	.ident	"GCC: (GNU) 4.3.3"
	.section	.text.__i686.get_pc_thunk.bx,"axG",@progbits,__i686.get_pc_thunk.bx,comdat
.globl __i686.get_pc_thunk.bx
	.hidden	__i686.get_pc_thunk.bx
	.type	__i686.get_pc_thunk.bx, @function
__i686.get_pc_thunk.bx:
	movl	(%esp), %ebx
	ret
	.section	.note.GNU-stack,"",@progbits
