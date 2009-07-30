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
	pushl %esp
	pushl 4(%esp)		/* eip */
	pushl %eax		/* orig_eax */
	pushf
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call wrapped_syscall

	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popf
	addl $0xC, %esp
	ret

__entry:
	/* before go int __entry, loader should have been pushed 3 things:
	   (from high addr to low addr (the order of push)) should be:
	   main_entry, logger_threshold, old_vdso_entry. */
	pushl %esp
	pushl 12(%esp)		/* eip */
	pushl $0		/* orig_eax */
	pushf
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	call injector_entry
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popf
	addl $20, %esp
	ret

__debug_entry:
	/* here, we should have reset all registers, and push eip at
	   the top of the stack */
	/* according to the gcc register saving protocal, caller
	   need to save only 3 registers. however I forget which 3.  */
	/* gs is special, at least in 2.6.25.15, ptrace cannot reset gs
	   for the target. debug_entry should do this */
	/* debug_entry has 3 args:
	 * 1. regs;
	 * 2. stack_user
	 * 3. stack_used.
	 * 2 and 3 are used for libpthread
	 */


	pushl %esp
	pushl 12(%esp)		/* eip */
	pushl $0		/* orig_eax */
	pushf
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call debug_entry

	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popf
	/* pop the sym_stack_used and sym_stack_user */
	addl $20, %esp
	ret


