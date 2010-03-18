/*
 * compiler.S
 * by WN @ Mar. 16, 2010
 */

#include <asm_offsets.h>

.text
/* log entry for unconditional direct branch */
/* ud branch should call this func the first time
 * it branch. after that, the recompile_ud_code_block will
 * recompile this code block, changes its exit address in the
 * code cache. each time a ud branch compiled, compiler will
 * set code_cache's last_ud_block. when the hit the exit code,
 * */
.globl ud_logger_entry
ud_logger_entry:
	/* switch stack! */
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	call recompile_ud_code_block
	popf
	popa
	/* switch stack back */
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmp *%fs:OFFSET_TARGET

.globl log_branch_target
log_branch_target:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	call heavy_log_branch_target
	popf
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmp *%fs:OFFSET_TARGET

// vim:ts=4:sw=4

