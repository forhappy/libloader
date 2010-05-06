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
 */
.globl __set_current_block_template_start
.globl __set_current_block_template_end
__set_current_block_template_start:
	movl $0xffffffff, %fs:OFFSET_CODE_CACHE_CURRENT_BLOCK
//	int3
__set_current_block_template_end:
	nop

.globl __log_phase_template_start
.globl __log_phase_template_end
.globl __log_phase_template_commit
.globl __log_phase_template_save_ebx
.globl __log_phase_template_save_return_addr
__log_phase_template_start:
	movl %eax, %fs:OFFSET_REG_SAVER1
__log_phase_template_save_ebx:
	movl %ebx, %fs:OFFSET_REG_SAVER2
	movl %fs:OFFSET_LOGGER_LOG_BUFFER_CURRENT, %ebx
	movl %fs:OFFSET_TARGET, %eax
	movl %eax, (%ebx)
	movl %fs:OFFSET_REG_SAVER2, %ebx
	lahf
__log_phase_template_commit:
	/* addl distrub eflags!!! */
	addl $4, %fs:OFFSET_LOGGER_LOG_BUFFER_CURRENT
	sahf
	movl %fs:OFFSET_REG_SAVER1, %eax
__log_phase_template_save_return_addr:
	movl $0xffffffff, %fs:OFFSET_LOGGER_CHECK_BUFFER_RETURN
	jmpl *%fs:OFFSET_LOGGER_CHECK_LOGGER_BUFFER
__log_phase_template_end:
	nop

.globl __real_branch_phase_template_start
.globl __real_branch_phase_template_end
__real_branch_phase_template_start:
	jmpl *%fs:OFFSET_REAL_BRANCH
__real_branch_phase_template_end:
	nop



/* this is only 'set target' phase. following is log phase */
#define DEF_Jxx_TEMPLATE(jxx)	\
.text;	\
.globl __##jxx##_template_start;	\
.globl __##jxx##_template_end;	\
.globl __##jxx##_template_taken_movl;	\
.globl __##jxx##_template_untaken_movl;	\
__##jxx##_template_start:	;	\
	jxx 1f;				\
__##jxx##_template_untaken_movl:	;	\
	movl $0xffffffff, %fs:OFFSET_TARGET;		\
	jmp 2f;				\
__##jxx##_template_taken_movl:			\
	1: movl $0xffffffff, %fs:OFFSET_TARGET;	\
	2: ;\
__##jxx##_template_end: nop

DEF_Jxx_TEMPLATE(jo)
DEF_Jxx_TEMPLATE(jno)
DEF_Jxx_TEMPLATE(jb)
DEF_Jxx_TEMPLATE(jnb)
DEF_Jxx_TEMPLATE(jz)
DEF_Jxx_TEMPLATE(jnz)
DEF_Jxx_TEMPLATE(jna)
DEF_Jxx_TEMPLATE(ja)

DEF_Jxx_TEMPLATE(js)
DEF_Jxx_TEMPLATE(jns)
DEF_Jxx_TEMPLATE(jp)
DEF_Jxx_TEMPLATE(jnp)
DEF_Jxx_TEMPLATE(jl)
DEF_Jxx_TEMPLATE(jge)
DEF_Jxx_TEMPLATE(jle)
DEF_Jxx_TEMPLATE(jg)

DEF_Jxx_TEMPLATE(jecxz)

	/* we needn't save anything: according
	 * to CURRENT_BLOCK we can decide where to return. However,
	 * saves its target address can make things simpler.
	 * for signal syscall, we don't care about whether
	 * the signal really raises inside it. the only thing
	 * we care is the log: if syscall mark before the signal
	 * mark, then it raise 'inside' syscall. If not, the signal
	 * raise before the syscall.
	 */
.globl __int80_syscall_template_start
.globl __int80_syscall_template_end
__int80_syscall_template_start:
	movl $0xffffffff, %fs:OFFSET_TARGET
	jmpl *%fs:OFFSET_INT80_SYSCALL_ENTRY
__int80_syscall_template_end:
	nop

.globl __vdso_syscall_template_start
.globl __vdso_syscall_template_end
__vdso_syscall_template_start:
	movl $0xffffffff, %fs:OFFSET_TARGET
	jmpl *%fs:OFFSET_VDSO_SYSCALL_ENTRY
__vdso_syscall_template_end:
	nop



.globl __direct_jmp_template_start
.globl __direct_jmp_template_end
__direct_jmp_template_start:
	movl $0xffffffff, %fs:OFFSET_TARGET
__direct_jmp_template_end:
	nop

.globl __direct_call_template_start
.globl __direct_call_template_end
.globl __direct_call_template_movl
__direct_call_template_start:
	/* don't use 0xffffffff: if not, as will use '6a ff' */
	pushl $0x12345678
__direct_call_template_movl:
	movl $0xffffffff, %fs:OFFSET_TARGET
__direct_call_template_end:
	nop

	/* ret is special: if signal raise inside the compiled branch
	 * and not just before the first instruction, it must be identified
	 * as 'after the branch'. Signal handler then check whether its eip
	 * between the top of template and the 'commit' instruction of log phase.
	 * if it is, it MUST restore eax and ebx then reset eip to the real branch phase.
	 * after that, signal handler should append ret target into log. */
.globl __ret_template_start
.globl __ret_template_end
__ret_template_start:
	popl %fs:OFFSET_TARGET
__ret_template_end:
	nop

.globl __retn_template_start
.globl __retn_template_end
__retn_template_start:
	movl %eax, %fs:OFFSET_REG_SAVER1
	movl (%esp), %eax
	movl %eax, %fs:OFFSET_TARGET
	movl %fs:OFFSET_REG_SAVER1, %eax
__retn_template_end:
	nop

.globl __rdtsc_template_start
.globl __rdtsc_template_end
.globl __rdtsc_template_save_return_addr
__rdtsc_template_start:
	movl $0xffffffff, %fs:OFFSET_TARGET
	rdtsc
	movl %ebx, %fs:OFFSET_REG_SAVER1
	movl %fs:OFFSET_LOGGER_LOG_BUFFER_CURRENT, %ebx
	movl %eax, (%ebx)
	movl %edx, 4(%ebx)
	/* this addl disturbs eflags!!! */
	lahf
	addl $8, %fs:OFFSET_LOGGER_LOG_BUFFER_CURRENT
	sahf
	movl (%ebx), %eax
	movl %fs:OFFSET_REG_SAVER1, %ebx
__rdtsc_template_save_return_addr:
	movl $0xffffffff, %fs:OFFSET_LOGGER_CHECK_BUFFER_RETURN
	jmpl *%fs:OFFSET_LOGGER_CHECK_LOGGER_BUFFER
__rdtsc_template_end:
	nop

.globl __save_eax_to_saver1_start
.globl __save_eax_to_saver1_end
__save_eax_to_saver1_start:
	movl %eax, %fs:OFFSET_REG_SAVER1
__save_eax_to_saver1_end:
	nop

.globl __set_target_restore_eax_start
.globl __set_target_restore_eax_end
__set_target_restore_eax_start:
	movl %eax, %fs:OFFSET_TARGET
	movl %fs:OFFSET_REG_SAVER1, %eax
__set_target_restore_eax_end:
	nop

.globl __vdso_syscall_inst_start
.globl __vdso_syscall_inst_end
__vdso_syscall_inst_start:
	call   *%gs:0x10
__vdso_syscall_inst_end:
	nop

	/* if signal arise: before the real processing, we MUST determine the
	 * exact instruction after that the signal arise. in the normal
	 * situation (signal raise between 2 normal instructions), we can
	 * determine the exact instruction by comparing the eip in signal frame
	 * and the CURRENT_BLOCK in %fs.  if eip < ori_code_end, then by (eip -
	 * __code + entry) we can get the exact instruction. if eip >
	 * ori_code_end, things become complex. We must consider the
	 * interaction between branch and log.
	 *
	 * Finally we decide that the log order is important. If the signal
	 * raise before we append log, it is identified as 'raise before
	 * branch'. If not, it is identified as 'raise after branch'. This
	 * simple policy moves the computation to replay phase. Some branched,
	 * such as 'call', operate the stack.  the replayer MUST compare the
	 * esp in signal frame and the real esp. If there're different, then
	 * the signal is actually raise after the 'call', the stack must be
	 * adjusted.  restoring the esp is not very hard, but restoring the
	 * traget address is different.  for 'call', it is not very hard.
	 * 'call' will return to the next instruction, the target address can
	 * also be computed.  for 'ret', the target address can be fetched from
	 * (%esp), for 'indirect call', the return address (put onto stack) is
	 * also its next instruction, but what's the target address?
	 *
	 * Here we show our solution:
	 * the instructions such as 'indirect call' and 'ret' will be compiled
	 * into 3 phase:
	 * 0. set target; (movl $0xffffffff, %fs:TARGET)
	 * 1. log;
	 * 2. take effect;
	 * 3. real transfer.
	 * log phase appends target address into log buffer. It needs following
	 * instructions:
	 *  movl %eax, %fs:REG_TMP
	 *  movl %fs:LOGGER_LOG_BUFFER_CURRENT, %eax
	 *  movl $0xffffffff, (%eax)
	 *  movl %fs:REG_TMP, %eax
	 *  addl $4, %fs:LOGGER_LOG_BUFFER_CURRENT
	 *  movl $0xffffffff, %fs:TMP_TARGET
	 *  jmp check_buffer
	 *
	 * Here, the buffer contain at least 4 bytes additional space.
	 * If signal raises inside the log phase, the signal handler must check the exact instruction
	 * it arise. if signal raise after the first movl but before the addl, then it must revert
	 * all works (reset eax and eip).
	 * by above method, we make the log phase atomic.
	 * the check_buffer shoule be:
	 *
	 *  movl %esp, %fs:OLD_ESP
	 *  movl %fs:ESP, %esp
	 *  do stuff
	 *  movl %fs:OLD_ESP, %esp
	 *  jmp *%fs:TMP_TARGET
	 *
	 * we don't care about signals raise after the first movl. following code are
	 * cacahe size checker, if they find the cache is not full, they will return to
	 * the original target. if signal raise after the comparation and before the flush,
	 * there is potential inconsistency.
	 * At the first situation, after the signal finally return,
	 * the sigreturn handler will flush cache if cache is full, so the check code
	 * got correct result. At the second situation, before it really fulsh cache,
	 * it will mask all signals, then recheck the buffer index. If signal raises before
	 * signal are blocked and after the comparation, the following comparation can still get
	 * correct result, and won't flush anything.
	 *
	 * some branch, such as jmp and call, doesn't have log phase. for 'jmp', put the signal
	 * before or after the instruction is nothing different. If record phase decide
	 * to identify signal as 'before jmp', it only work is adjust eip. If it decide to identify
	 * signal as 'after jmp', it still need to adjust eip. During replay, do the same work
	 * is enough. for 'call', replay phase can infer its place by comparing esp.
	 *
	 * 'take effect' phase is used only for such instruction which have side effect and
	 * need to be logged, such as 'ret' and indirect call. 'ret' will be compiled as
	 * an 'popl' instruction, and indirect call will be compiled as a 'push' instruction.
	 * if signal raise after log phase but before 'take effect' phase, it will be identified as
	 * 'after the branch', but the stack has not been modified. In record phase, if the eip is
	 * not between the effective instruction (exclusive) and the end of the code piece, and not 
	 * at the head of the new block, 
	 * it will reset eip to the real branch. if not (after the effective inst, it won't trigger
	 * tls code), it will reset eip to the branch target.
	 *
	 * so, for such instructions, if eip in signal frame is after the 'take effect' instruction and
	 * before the last instruction, the eip addr will be adjusted to the branch target; if not,
	 * the eip addr will be adjusted to the branch instruction.
	 *
	 * Then there have an inconsistance: if signal raises after log phase but before the effective
	 * instruction, it will be identified as 'before the signal' during record, and identified as
	 * 'after the signal' during replay. we left the work in replay phase: if it meet such an
	 * instructiion, it mush save stack values in and make it recoverable. After it execute
	 * the instruction, before following execution, it must check signal status. if following
	 * signal raise after it (according to its eip (it is set during record phase, eip point to
	 * the instruction or its target(its target has been saved during its replay execution))),
	 * it will check the signal eip. if it is points to the branch itself, then replayer must
	 * roll-back the branch by reset esp, move saved stack value back and properly set signal frame
	 * (copy from log).
	 *
	 * 
	 * summary:
	 *  when compiling branch, the branch instructions (exclude unconditional direct branches)
	 *  are diveide into 4 phase:
	 *   1. set target (one inst)
	 *   2. log phase
	 *   3. take effect;
	 *   4. real branch.
	 * when signal arises, the signal handler should do following things:
	 *   1. check the eip in signal frame, if it is inside the critical code of log phase,
	 *      handler must reset eax from REG_TMP, then reset the eip to the head of
	 *      the compiled branch instructions;
	 *   2. save the signal frame and make a fake frame. The fake frame's eip is determined by
	 *      original eip: for no-side effect instructions, if eip is before the first critical log
	 *      instruction (less than it) then reset the frame's eip to the branch instruction it self.
	 *      if not, set it to its branch target; for those 'have side effect' branch (indirect call
	 *      and ret), if eip is after the 'take effect' instruction, or it is the first instruction
	 *      of compiled branch target, set the fake eip to the branch target, if not, set the fake eip
	 *      to the branch itself.
	 *   3. when signal return, retrives the saved frame. (in fact, only the eip is changed, so we can
	 *      save eip only. however, some tls state also need to be saved).
	 *
	 *   during replay, when hit side-effect branch, must save the stack value. after that, if
	 *   the next log entry is a signal mark, and points to the branch instruction (according to esp
	 *   we can check this), this instruction must be reverted, and reexec when signal return.
	 *
	 */

/* don't generate executable stack */
.section        .note.GNU-stack,"",@progbits

