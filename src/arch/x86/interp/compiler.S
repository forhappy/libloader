/*
 * compiler.S
 * by WN @ Mar. 22, 2010
 */

#include <asm_offsets.h>

.text
.globl real_branch
real_branch:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	call do_real_branch
	popfl
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmpl *%fs:OFFSET_TARGET


/* user_branch is the patched '_start' code. The loader
 * needs to patch the '_start' code to direct control flow
 * to user_branch */
.globl user_branch
user_branch:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	call do_user_branch
	popfl
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	/* tpd->target should has been set by do_user_branch */
	jmpl *%fs:OFFSET_REAL_BRANCH

/*
 * below entries are for gdbserver:
 * gdbserver set OFFSET_TARGET and redirect code to it.
 */
.globl replay_patch_block
replay_patch_block:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	call do_replay_patch_block
	popfl
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmpl *%fs:OFFSET_REAL_BRANCH

.globl replay_unpatch_block
replay_unpatch_block:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	call do_replay_unpatch_block
	popfl
	popa
	movl %fs:OFFSET_OLD_STACK_TOP, %esp
	jmpl *%fs:OFFSET_REAL_BRANCH

.globl replay_is_branch_inst
replay_is_branch_inst:
	movl %esp, %fs:OFFSET_OLD_STACK_TOP
	movl %fs:OFFSET_STACK_TOP, %esp
	pusha
	pushf
	pushl $0x0202
	popfl
	call do_replay_is_branch_inst
	/* never comes here */
	int3

/* don't generate executable stack */
.section        .note.GNU-stack,"",@progbits

