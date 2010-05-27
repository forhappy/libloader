/*
 * compiler.S
 * by WN @ Mar. 16, 2010
 */

#include <asm_offsets.h>

.text

.globl check_logger_buffer
check_logger_buffer:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	/* defined in interp/logger.c */
	call do_check_logger_buffer
	popf
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmpl *%fs:OFFSET_LOGGER_CHECK_BUFFER_RETURN

.globl int80_syscall_entry
int80_syscall_entry:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	call pre_log_syscall_int80
	cmp $0, %eax
	jne 1f

	popf
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp

	int $0x80

	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	1: call post_log_syscall_int80
	popf
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp

	jmpl *%fs:OFFSET_REAL_BRANCH
	nop

.globl vdso_syscall_entry
vdso_syscall_entry:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	call pre_log_syscall_vdso
	cmp $0, %eax
	jne 1f

	popf
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp

	call *%fs:OFFSET_REAL_VDSO_SYSCALL_ENTRY

	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	1: call post_log_syscall_vdso
	popf
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp

	jmpl *%fs:OFFSET_REAL_BRANCH
	nop

/* don't generate executable stack */
.section        .note.GNU-stack,"",@progbits


// vim:ts=4:sw=4

