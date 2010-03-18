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
__ud_logger_entry_before_pusha:
	pusha
__ud_logger_entry_before_pushf:
	pushf
	call compile_code_block
	popf
__ud_logger_entry_before_popa:
	popa
	/* switch stack back */
__ud_logger_entry_before_switch_esp:
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmp *%fs:OFFSET_TARGET

.globl log_branch_target
log_branch_target:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
__log_branch_entry_before_pusha:
	pusha
__log_branch_entry_before_pushf:
	pushf
	call heavy_log_branch_target
	call compile_code_block
	popf
__log_branch_entry_before_popa:
	popa
__log_branch_entry_before_switch_esp:
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmp *%fs:OFFSET_TARGET

// vim:ts=4:sw=4

