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

	/* effective inst is used for signal handling. if signal eip
	 * less than effective_inst, then the signal raise befor the branch;
	 * if else, the signal raise after the branch.
	 * this is useful for some instructions which have side effects, such as call.
	 */
#define DEF_Jxx_TEMPLATE(jxx)	\
.text;	\
.globl __##jxx##_template_start;	\
.globl __##jxx##_template_end;	\
.globl __##jxx##_template_taken_movl;	\
.globl __##jxx##_template_untaken_movl;	\
.globl __##jxx##_template_effective_inst;	\
__##jxx##_template_start:	;	\
	movl %eax, %fs:OFFSET_TARGET;	\
__##jxx##_template_effective_inst:	;	\
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

	/* we needn't save anything: according
	 * to CURRENT_BLOCK we can decide where to return. However,
	 * saves its target address can make things simpler.
	 * for signal syscall, we don't care about whether
	 * the signal really raises inside it. the only thing
	 * we care is the log: if syscall mark before the signal
	 * mark, then it raise 'inside' syscall. If not, the signal
	 * raise before the syscall.
	 */
__int80_syscall_template_start:
	movl %eax, %fs:OFFSET_TARGET
__int80_syscall_template_movl:
	movl $0xffffffff, %eax
	xchgl %eax, %fs:OFFSET_TARGET
	jmpl *%fs:OFFSET_SYSCALL_ENTRY
__int80_syscall_template_end:
	nop


	/* if signal arise:
	 * before the real processing, we MUST determine the exact instruction
	 * after that the signal arise. in the normal situation (signal raise
	 * between 2 normal instructions), we can determine the exact instruction
	 * by comparing the eip in signal frame and the CURRENT_BLOCK in %fs.
	 * if eip < ori_code_end, then by (eip - __code + entry) we can get the exact
	 * instruction. if eip > ori_code_end: if eip < xxx_effect_inst, then signal
	 * raise before the branch; if not, then signal arise after the branch.
	 * for system call, we don't need determine the exact signal point by instruction
	 * address. during replay, if signal mark before the system call mark, then
	 * the signal is identified as 'raise before system call'; if signal mark
	 * after syscall mark and before syscall data, then it raise inside system call.
	 * if signal mark after all syscall data, then it raise after system call.
	 */
	/* The log problem: when we append an entry into log, we need to do 2 things:
	 * 1: adjust the buffer index, 2: append the entry. These 2 things can't be done
	 * by one instruction, so signal may raise between those instructions.
	 * because we */
