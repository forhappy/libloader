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
	
	pushl $0		/* eip */
	pushl $0		/* orig_eax */
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	call __before_syscall
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	addl $0x8, %esp

	call __vsyscall

	pushl $0		/* eip */
	pushl $0		/* orig_eax */
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	call __after_syscall
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	addl $0x8, %esp
	ret

__entry:
	call injector_entry
	addl $0x8, %esp
	ret

