/*
 * entry.S
 * by WN @ Jun. 23, 2009
 */

.section .data
.section .text

.type show_help_entry, @function
.global show_help_entry
show_help_entry:
	call show_help
	movl $1, %eax
	int $0x80
	ret

syscall_wrapper_entry:
	call __vsyscall
	ret

__entry:
	call injector_entry
	addl $0x8, %esp
	ret

