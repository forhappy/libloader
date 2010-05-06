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
	int $0x80
	jmpl *%fs:OFFSET_REAL_BRANCH
	nop

.globl vdso_syscall_entry
vdso_syscall_entry:
	call *%fs:OFFSET_REAL_VDSO_SYSCALL_ENTRY
	jmpl *%fs:OFFSET_REAL_BRANCH
	nop

/* don't generate executable stack */
.section        .note.GNU-stack,"",@progbits


// vim:ts=4:sw=4

