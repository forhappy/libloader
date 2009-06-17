.section .text

.type show_help_entry, @function
.global show_help_entry

.type syscall_wrapper_entrace, @function
.global syscall_wrapper_entrace



.section .data
.global vdso_entrace
.type vdso_entrace, @object
.global vdso_ehdr
.type vdso_ehdr, @object


vdso_entrace:
	.word 0
vdso_ehdr:
	.word 0
string_start:
	.asciz "syscall start\n"
string_end:
	.asciz "syscall end\n"
.previous	

show_help_entry:
	call show_help
	movl $1, %eax
	int $0x80
	ret


syscall_wrapper_entrace:
	call syscall_wrapper
	int $0x80
	ret

