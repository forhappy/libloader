/*
 * arch_syscall.S
 * by WN @ Nov, 22, 2010
 */

#ifndef ARCH_SYSCALL_S
#define ARCH_SYSCALL_S

#define PRE_RET_NORMAL	(0)
#define PRE_RET_DIRECT_POST	(1)
#define PRE_RET_NOSIGNAL	(2)
#define PRE_RET_CLONE	(3)

#define POST_RET_NORMAL	(0)
#define POST_RET_UNTRACE	(1)

#ifndef __ASSEMBLY__

#include <common/defs.h>
#include <xasm/processor.h>

extern void
arch_syscall_e(void) ATTR_HIDDEN;

extern int
arch_pre_syscall(struct pusha_regs * r);

extern int
arch_post_syscall(struct pusha_regs * r);

extern int syscall_unblock_signal[] ATTR_HIDDEN;
extern int syscall_inst[] ATTR_HIDDEN;
extern int syscall_block_signal[] ATTR_HIDDEN;
extern int after_syscall_inst[] ATTR_HIDDEN;

#else

#include <interp/rebranch_asm_offset.h>
#include <interp/asm_helper.S.h>
#include <xasm/unistd.h>


#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */

.text
.globl arch_syscall_e

arch_syscall_e:
	ENTER_ENTRY
	call arch_pre_syscall
	cmp $PRE_RET_NORMAL, %eax
	je normal_processing
	cmp $PRE_RET_DIRECT_POST, %eax
	je post_processing
	cmp $PRE_RET_NOSIGNAL, %eax
	je nosignal_processing
	cmp $PRE_RET_CLONE, %eax
	je clone_processing
	int3


.globl syscall_unblock_signal
.globl syscall_inst
.globl after_syscall_inst
.globl syscall_block_signal

normal_processing:
	/* unblock signal */
	/* unblock signal should be done after stack switch */
	/* make sure all signals raise in user stack */

	PREPARE_EXIT
	/* if modify these code, signal_handler.c MUST be modify
	 * accordingly. */
	movl %eax, %fs:OFFSET_CC_REG_SAVER_1
	movl %ebx, %fs:OFFSET_CC_REG_SAVER_2
	movl %ecx, %fs:OFFSET_CC_REG_SAVER_3
	movl %edx, %fs:OFFSET_CC_REG_SAVER_4
	movl %esi, %fs:OFFSET_CC_REG_SAVER_5
	lahf
	movl %eax, %fs:OFFSET_CC_REG_SAVER_6

	movl $__NR_rt_sigprocmask, %eax
	movl %fs:OFFSET_TD_START_ADDR, %ebx
	leal OFFSET_TPD_SIGMASK_USER(%ebx), %ecx
	movl $0, %edx
	movl $SIG_SETMASK, %ebx
	movl $8, %esi
syscall_unblock_signal:
	int $0x80

	movl %fs:OFFSET_CC_REG_SAVER_6, %eax
	sahf
	movl %fs:OFFSET_CC_REG_SAVER_1, %eax 
	movl %fs:OFFSET_CC_REG_SAVER_2, %ebx 
	movl %fs:OFFSET_CC_REG_SAVER_3, %ecx 
	movl %fs:OFFSET_CC_REG_SAVER_4, %edx 
	movl %fs:OFFSET_CC_REG_SAVER_5, %esi 

syscall_inst:
	int $0x80
after_syscall_inst:

	movl %eax, %fs:OFFSET_CC_REG_SAVER_1
	movl %ebx, %fs:OFFSET_CC_REG_SAVER_2
	movl %ecx, %fs:OFFSET_CC_REG_SAVER_3
	movl %edx, %fs:OFFSET_CC_REG_SAVER_4
	movl %esi, %fs:OFFSET_CC_REG_SAVER_5
	lahf
	movl %eax, %fs:OFFSET_CC_REG_SAVER_6

	/* and reblock signal should be done before stack switch */
	movl $__NR_rt_sigprocmask, %eax
	movl %fs:OFFSET_TD_START_ADDR, %ebx
	leal OFFSET_TPD_SIGMASK_REBRANCH(%ebx), %ecx
	leal OFFSET_TPD_SIGMASK_USER(%ebx), %edx
	movl $SIG_SETMASK, %ebx
	movl $8, %esi
	int $0x80
syscall_block_signal:
	movl %fs:OFFSET_CC_REG_SAVER_6, %eax
	sahf
	movl %fs:OFFSET_CC_REG_SAVER_1, %eax 
	movl %fs:OFFSET_CC_REG_SAVER_2, %ebx 
	movl %fs:OFFSET_CC_REG_SAVER_3, %ecx 
	movl %fs:OFFSET_CC_REG_SAVER_4, %edx 
	movl %fs:OFFSET_CC_REG_SAVER_5, %esi 

	ENTER_ENTRY
post_processing:
	call arch_post_syscall
	cmpl $POST_RET_NORMAL, %eax
	je normal_exit
	cmpl $POST_RET_UNTRACE, %eax
	je untrace_exit
	int3

normal_exit:
	EXIT_TO(OFFSET_CC_COMPILER)
untrace_exit:
	EXIT_TO(OFFSET_CC_TARGET)




nosignal_processing:
	PREPARE_EXIT
	int $0x80
	ENTER_ENTRY
	jmp post_processing


clone_processing:
	PREPARE_EXIT
	int $0x80
	pushf
	cmp $0, %eax
	jne clone_parent_or_fail_processing
	/* clone child processing */
	popf
	/* child's tls has not complete */
	/* don't switch stack */
	pusha
	pushf
	pushl $0x0202
	popfl
	push %esp
	call child_setup_tls
	addl $4, %esp
	popf
	popa
	ENTER_ENTRY
	call post_clone_child
	EXIT_TO(OFFSET_CC_COMPILER)



clone_parent_or_fail_processing:
	popf
	ENTER_ENTRY
	jmp post_processing;

/* don't generate executable stack */
.section        .note.GNU-stack,"",@progbits

#endif
#endif

// vim:ts=4:sw=4
