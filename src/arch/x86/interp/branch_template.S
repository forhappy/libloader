/*
 * branch_templates.S
 * by WN @ Mar. 18, 2010
 */

#include <asm_offsets.h>

/* 
 * __set_current_block_template should be copied at the
 * head of each compiled code block. We use
 * tps->code_block.current_block for signal handling:
 * the signal handler knows current code block, according
 * to the difference between eip in signal stack and the
 * address of compiled code, signal handler can determine
 * the exact instruction before which signal arrives.
 *
 * However, if signal raises during the template code, the
 * identification is not so easy.
 * The signal handler should check the specific instruction of
 * eip in signal frame.
 * if it is 'movl %eax, %fs:OFFSET_CODE_CACHE_CURRENT_BLOCK',
 * then it knows the value of eax has been changed. It shoule restore signal
 * frame using reg_saver.
 * if it is 'movl %fs:OFFSET_REG_SAVER, %eax', signal handler should
 * do the same thing.
 * in both situation, the signal itself is identified as raised after
 * the branch instruction and before the first instruction of the new block.
 */
.globl __set_current_block_template_start
.globl __set_current_block_template_end
.globl __set_current_block_template_movl
.globl __set_current_block_template_before_set_ccb
.globl __set_current_block_template_before_restore_eax
__set_current_block_template_start:
	movl %eax, %fs:OFFSET_REG_SAVER
__set_current_block_template_movl:
	movl $0xffffffff, %eax
__set_current_block_template_before_set_ccb:
	movl %eax, %fs:OFFSET_CODE_CACHE_CURRENT_BLOCK
__set_current_block_template_before_restore_eax:
	movl %fs:OFFSET_REG_SAVER, %eax
__set_current_block_template_end:
	nop

#define DEF_Jxx_TEMPLATE(jxx)	\
.text;	\
.globl __##jxx##_template_start;	\
.globl __##jxx##_template_end;	\
.globl __##jxx##_template_taken_movl;	\
.globl __##jxx##_template_untaken_movl;	\
__##jxx##_template_start:	;	\
	movl %eax, %fs:OFFSET_TARGET;	\
	jxx 1f;				\
__##jxx##_template_untaken_movl:	;	\
	movl $0xffffffff, %eax;		\
	jmp 2f;				\
__##jxx##_template_taken_movl:			\
	1: movl $0xffffffff, %eax;	\
	2: xchgl %eax, %fs:OFFSET_TARGET;  \
	jmpl *%fs:OFFSET_LOGGER_LOG_BRANCH_TARGET;	\
__##jxx##_template_end: nop



DEF_Jxx_TEMPLATE(jo)
DEF_Jxx_TEMPLATE(jno)
DEF_Jxx_TEMPLATE(jb)
DEF_Jxx_TEMPLATE(jnb)
DEF_Jxx_TEMPLATE(jz)
DEF_Jxx_TEMPLATE(jnz)
DEF_Jxx_TEMPLATE(jna)
DEF_Jxx_TEMPLATE(ja)

