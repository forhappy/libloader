/* 
 * compiler.c
 * by WN @ Mar. 16, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>
#include <interp/mm.h>
#include <interp/dict.h>
#include <interp/code_cache.h>
#include <xasm/compiler.h>
#include <xasm/tls.h>
#include <xasm/debug.h>
#include <xasm/string.h>
#include <asm_offsets.h>

#define MAX_PATCH_SIZE	(256)

/* return value is the position of the value */
static __AI uint32_t *
reset_movl_imm(uint8_t * inst, uint32_t data)
{
	uint32_t * val_addr;
	if ((inst[0] == 0x64) && (inst[1] == 0xc7) && (inst[2] == 0x05)) {
		/* this is movl $0xffffffff, %fs:0x12345678 */
		val_addr = ((uint32_t*)(inst + 7));
	} else if ((inst[0] == 0xc7) && (inst[1] == 0x05)) {
		/* this is movl $0xffffffff, 0x12345678 */
		val_addr = ((uint32_t*)(inst + 6));
	} else {
		assert(0);
	}
	*val_addr = data;
	return val_addr;
}

static __AI uint32_t *
reset_push_imm(uint8_t * inst, uint32_t data)
{
	uint32_t * val_addr;
	if (inst[0] == 0x68) {
		val_addr = (uint32_t*)(&inst[1]);
	} else {
		assert(0);
	}
	*val_addr = data;
	return val_addr;
}

#define template_sym(s)		extern ATTR_HIDDEN uint8_t s[]

template_sym(__set_current_block_template_start);
template_sym(__set_current_block_template_end);
template_sym(__log_phase_template_start);
template_sym(__log_phase_template_end);
template_sym(__log_phase_template_commit);
template_sym(__log_phase_template_save_ebx);
template_sym(__log_phase_template_save_return_addr);
template_sym(__real_branch_phase_template_start);
template_sym(__real_branch_phase_template_end);


#define ptr_length(a, b)	((uintptr_t)(a) - (uintptr_t)(b))
#define template_offset(t, x)	ptr_length(t##_##x, t##_start)
#define template_sz(x)	template_offset(x, end)
#define inst_in_template(target, t, x)	(((uint8_t*)(target)) + template_offset(t, x))

#define MAX_PATCH_SIZE	(256)
/* see recompile_ud , an unconditional branch should
 * have at least 6 bytes */
#define SMALLEST_PATCH_SIZE	(6)

/* return value is the address of the value in the critical movl */
static __AI uint32_t *
copy_log_phase(uint8_t * target)
{
	memcpy(target, __log_phase_template_start,
			template_sz(__log_phase_template));
	uint8_t * movl_inst = inst_in_template(target,
			__log_phase_template, save_return_addr);
	return reset_movl_imm(movl_inst, (uint32_t)(uintptr_t)inst_in_template(target,
				__log_phase_template, end));
}

#define MODRM_MOD(x)	(((x) & 0xc0)>>6)
#define MODRM_REG(x)	(((x) & 0x38)>>3)
#define MODRM_RM(x)		(((x) & 0x7))
#define BUILD_MODRM(__mod, __reg, __rm)	(((__mod) << 6) + ((__reg) << 3) + ((__rm)))

/* according to the value of modrm (RM and MOD field), generate following code:
 * for register:
 * movl %exx, %fs:OFFSET_TARGET
 *
 * for memory:
 * movl %eax, %fs:OFFSET_REG_SAVER1
 * movl (xxxxx), %eax
 * movl %eax, %fs:OFFSET_TARGET
 * movl %fs:OFFSET_REG_SAVER1, %eax
 *
 * then, copy log phase code.
 *
 * info_sz is the size of the extra info of original instruction.
 * */
static uint8_t *
compile_modrm_target(uint8_t * patch_code, uint8_t * pmodrm,
		uint32_t ** log_phase_retaddr_fix, int * pinst_sz)
{
	uint8_t * pos = patch_code;
	uint8_t modrm = *pmodrm;
	/* modrm */
	(*pinst_sz) += 1;
	if (MODRM_MOD(modrm) == 3) {
		/* target is from reg */
		switch (MODRM_RM(modrm)) {
			case 0: {
				/* this is eax */
				/* movl %eax, %fs:???? */
				pos[0] = 0x64;
				pos[1] = 0xa3;
				pos += 2;
				break;
			}
			default: {
				pos[0] = 0x64;
				pos[1] = 0x89;
				/* mod = 0, reg=MODRM_RM(modrm), rm=101 */
				pos[2] = BUILD_MODRM(0, MODRM_RM(modrm), 5);
				pos += 3;
			}
		}
		*(uint32_t*)(pos) = OFFSET_TARGET;
		pos += 4;
	} else {
		/* this is normal memory access */
		template_sym(__save_eax_to_saver1_start);
		template_sym(__save_eax_to_saver1_end);
		template_sym(__set_target_restore_eax_start);
		template_sym(__set_target_restore_eax_end);
		/* copy movl %eax, %fs:OFFSET_REG_SAVER1 */
		memcpy(pos, __save_eax_to_saver1_start,
				template_sz(__save_eax_to_saver1));
		pos += template_sz(__save_eax_to_saver1);

		/* move target to eax */
		/* use 0x8b: movl (xxx), %exx */
		*(pos++) = 0x8b;
		uint8_t new_modrm = BUILD_MODRM(
				MODRM_MOD(modrm),
				0,
				MODRM_RM(modrm));

		*(pos++) = new_modrm;
		pmodrm ++;
		bool_t have_sib = FALSE;
		uint8_t sib = 0;
		if (MODRM_RM(new_modrm) == 4) {
			/* has following SIB */
			have_sib = TRUE;
			sib = pmodrm[0];
			pmodrm ++;

			*(pos++) = sib;
			/* sib */
			(*pinst_sz) ++;
		}
		if (MODRM_MOD(new_modrm) == 1) {
			/* have disp8 */
			*(pos++) = *pmodrm;
			(*pinst_sz) ++;
		} else if (MODRM_MOD(new_modrm) == 2) {
			/* have disp32 */
			*(uint32_t*)(pos) = *((uint32_t*)(pmodrm));
			pos += 4;
			(*pinst_sz) += 4;
		}

		if (have_sib) {
			uint8_t base = sib & 0x7;
			if (base == 5) {
				if (MODRM_MOD(new_modrm) == 0) {
					/* disp 32 with no base */
					*(uint32_t*)(pos) = *((uint32_t*)(pmodrm));
					pos += 4;
					(*pinst_sz) += 4;
				}
			}
		}

		/* copy "set target" */
		memcpy(pos, __set_target_restore_eax_start,
				template_sz(__set_target_restore_eax));
		pos += template_sz(__set_target_restore_eax);
	}
	/* copy 'log phase' */
	*log_phase_retaddr_fix = copy_log_phase(pos);
	pos += template_sz(__log_phase_template);
	return pos;
}

static int
compile_branch(uint8_t * patch_code, uint8_t * branch,
		enum exit_type * pexit_type, uint32_t ** log_phase_retaddr_fix,
		int * recompile_branch_offset)
{
#warning unfinished
	TRACE(COMPILER, "compiling branch %p\n", branch);
	*log_phase_retaddr_fix = NULL;
	*recompile_branch_offset = 0;
	uint8_t inst1 = branch[0];
	uint8_t inst2 = branch[1];
	uint8_t inst3 = branch[2];
	const int real_branch_template_sz =
		template_sz(__real_branch_phase_template);
	const int log_phase_template_sz =
		template_sz(__log_phase_template);
	switch (inst1) {
#define COMP_Jxx(jxx, untaken_exit, taken_exit) do {	\
	extern uint8_t __##jxx##_template_start[] ATTR_HIDDEN; 		\
	extern uint8_t __##jxx##_template_end[] ATTR_HIDDEN; 		\
	extern uint8_t __##jxx##_template_taken_movl[] ATTR_HIDDEN; \
	extern uint8_t __##jxx##_template_untaken_movl[] ATTR_HIDDEN; \
	*pexit_type = EXIT_COND_DIRECT;								\
	void * ___untaken_exit = untaken_exit;							\
	void * ___taken_exit = taken_exit;								\
	int ___patch_sz = template_sz(__##jxx##_template) + 				\
					log_phase_template_sz +			\
					real_branch_template_sz;	\
	assert(___patch_sz <= MAX_PATCH_SIZE);							\
	memcpy(patch_code, (void*)__##jxx##_template_start, template_sz(__##jxx##_template));\
	reset_movl_imm(inst_in_template(patch_code, __##jxx##_template, taken_movl), \
			(uint32_t)(___taken_exit)); \
	reset_movl_imm(inst_in_template(patch_code, __##jxx##_template, untaken_movl), \
			(uint32_t)(___untaken_exit)); \
	*log_phase_retaddr_fix = copy_log_phase(patch_code + template_sz(__##jxx##_template));\
	memcpy(patch_code + \
			template_sz(__##jxx##_template) + \
			log_phase_template_sz,	\
			__real_branch_phase_template_start, \
			real_branch_template_sz);	\
	return ___patch_sz;\
} while(0)

#define COMP_Jxx_8b(jxx) COMP_Jxx(jxx, branch + 2, branch + 2 + *((int8_t*)(branch + 1)))
#define CASE_Jxx_8b(num, jxx)	case (num): {COMP_Jxx_8b(jxx); break;}

		CASE_Jxx_8b(0x70, jo)
		CASE_Jxx_8b(0x71, jno)
		CASE_Jxx_8b(0x72, jb)
		CASE_Jxx_8b(0x73, jnb)
		CASE_Jxx_8b(0x74, jz)
		CASE_Jxx_8b(0x75, jnz)
		CASE_Jxx_8b(0x76, jna)
		CASE_Jxx_8b(0x77, ja)

		CASE_Jxx_8b(0x78, js)
		CASE_Jxx_8b(0x79, jns)
		CASE_Jxx_8b(0x7a, jp)
		CASE_Jxx_8b(0x7b, jnp)
		CASE_Jxx_8b(0x7c, jl)
		CASE_Jxx_8b(0x7d, jge)
		CASE_Jxx_8b(0x7e, jle)
		CASE_Jxx_8b(0x7f, jg)

		CASE_Jxx_8b(0xe3, jecxz)

#undef COMP_Jxx_8b
#undef CASE_Jxx_8b

		case 0xe9:
		case 0xeb: {
			template_sym(__direct_jmp_template_start);
			template_sym(__direct_jmp_template_end);
			/* 0xeb is short jmp, 0xe9 is long jmp */
			*pexit_type = EXIT_UNCOND_DIRECT;
			int patch_sz = template_sz(__direct_jmp_template) +
				real_branch_template_sz;
			void * target;
			if (inst1 == 0xeb)
				target = branch + 2 + *((int8_t*)(branch + 1));
			else
				target = branch + 5 + *((int32_t*)(branch + 1));
			memcpy(patch_code, __direct_jmp_template_start,
					template_sz(__direct_jmp_template));
			reset_movl_imm(patch_code, (uint32_t)(target));
			/* no log phase */
			memcpy(patch_code + template_sz(__direct_jmp_template),
					__real_branch_phase_template_start,
					real_branch_template_sz);
			*recompile_branch_offset = 0;
			return patch_sz;
		}

		case 0xe8: {
			template_sym(__direct_call_template_start);
			template_sym(__direct_call_template_end);
			template_sym(__direct_call_template_movl);
			/* this is 'call rel32' */
			*pexit_type = EXIT_UNCOND_DIRECT;
			int patch_sz = template_sz(__direct_call_template) +
				real_branch_template_sz;
			void * target = branch + 5 + *((int32_t*)(branch + 1));
			void * next_inst = branch + 5;
			memcpy(patch_code, __direct_call_template_start,
					template_sz(__direct_call_template));
			reset_push_imm(patch_code, (uint32_t)(next_inst));
			reset_movl_imm(inst_in_template(patch_code, __direct_call_template, movl),
					            (uint32_t)target);
			/* no log phase */
			memcpy(patch_code + template_sz(__direct_call_template),
					__real_branch_phase_template_start,
					real_branch_template_sz);
			*recompile_branch_offset = template_offset(__direct_call_template,
					movl);
			return patch_sz;
		}

		case 0xc3: {
			template_sym(__ret_template_start);
			template_sym(__ret_template_end);
			/* this is normal 'ret' */
			*pexit_type = EXIT_UNCOND_INDIRECT;
			int tmpsz = template_sz(__ret_template);
			/* ret is special: it should have an 'effect phase' but we
			 * cut it out. see comments in branch_template.S */
			int patch_sz = tmpsz +
				log_phase_template_sz +
				real_branch_template_sz;
			memcpy(patch_code, (void*)__ret_template_start,
					tmpsz);
			*log_phase_retaddr_fix = copy_log_phase(patch_code +
					tmpsz);
			memcpy(patch_code + tmpsz + log_phase_template_sz,
					__real_branch_phase_template_start,
					real_branch_template_sz);
			return patch_sz;
		}

		case 0xc2: {
			/* this is 'ret imm16' */
			template_sym(__retn_template_start);
			template_sym(__retn_template_end);
			*pexit_type = EXIT_UNCOND_INDIRECT;
			int tmpsz = template_sz(__retn_template);
			memcpy(patch_code, (void*)__retn_template_start,
					tmpsz);
			/* then is log phase */
			*log_phase_retaddr_fix = copy_log_phase(patch_code +
					tmpsz);
			/* then is the effect phase */
#warning This may incorrect
			uint8_t * eff_ptr = patch_code + tmpsz + log_phase_template_sz;
			/* this is 'addl imm32, %esp', 6 bytes */
			eff_ptr[0] = 0x81;
			eff_ptr[1] = 0xc4;
			*((int32_t*)(eff_ptr + 2)) = *((int16_t*)(branch + 1)) + 4;
			memcpy(patch_code + tmpsz + log_phase_template_sz + 6,
					__real_branch_phase_template_start,
					real_branch_template_sz);
			return tmpsz + log_phase_template_sz + 6 + real_branch_template_sz;
		}

		case 0xff: {
			/* this is group 5, may be: calln, callf, jmpn, jmpf
			 * according to modrm. we needn't to support callf and jmpf */
			uint8_t modrm = inst2;
			*pexit_type = EXIT_UNCOND_INDIRECT;

			/* the instruction (0xff) */
			int inst_sz = 1;
			/* first, we move the target address into fs:OFFSET_TARGET */
			uint8_t * ptr = compile_modrm_target(patch_code, branch + 1,
					log_phase_retaddr_fix, &inst_sz);
			/* this is 'take effect' section */
			switch (MODRM_REG(modrm)) {
				case (2) : {
					/* this is calln */
					/* we need to push return address */
					*(ptr++) = 0x68;
					*((uint32_t*)(ptr)) = (uint32_t)(branch + inst_sz);
					ptr += 4;
					break;
				}
				case (4) : {
					/* this is jmpn */
					/* do nothing */
					break;
				}
				default: {
					FATAL(COMPILER, "doesn't support 0xff 0x%x\n", modrm);
				}
			}
			/* after that, is 'real branch' */
			memcpy(ptr, __real_branch_phase_template_start,
					real_branch_template_sz);
			return (uintptr_t)(ptr) - (uintptr_t)(patch_code)
				+ real_branch_template_sz;
		}


		case 0x0f: {
			switch (inst2) {
#define COMP_Jxx_32b(jxx) COMP_Jxx(jxx, branch + 6, branch + 6 + *((int32_t*)(branch + 2)))
#define CASE_Jxx_32b(num, jxx)	case (num): {COMP_Jxx_32b(jxx); break;}

				CASE_Jxx_32b(0x80, jo)
				CASE_Jxx_32b(0x81, jno)
				CASE_Jxx_32b(0x82, jb)
				CASE_Jxx_32b(0x83, jnb)
				CASE_Jxx_32b(0x84, jz)
				CASE_Jxx_32b(0x85, jnz)
				CASE_Jxx_32b(0x86, jna)
				CASE_Jxx_32b(0x87, ja)

				CASE_Jxx_32b(0x88, js)
				CASE_Jxx_32b(0x89, jns)
				CASE_Jxx_32b(0x8a, jp)
				CASE_Jxx_32b(0x8b, jnp)
				CASE_Jxx_32b(0x8c, jl)
				CASE_Jxx_32b(0x8d, jge)
				CASE_Jxx_32b(0x8e, jle)
				CASE_Jxx_32b(0x8f, jg)

#undef CASE_Jxx_32b
#undef COMP_Jxx_32b
#undef COMP_Jxx
				case 0x31: {
					/* this is rdtsc */
					template_sym(__rdtsc_template_start);
					template_sym(__rdtsc_template_end);
					template_sym(__rdtsc_template_save_return_addr);
					int tmpsz = template_sz(__rdtsc_template);
					int patch_sz = tmpsz +
						real_branch_template_sz;
					memcpy(patch_code, (void*)__rdtsc_template_start, tmpsz);
					reset_movl_imm(patch_code, (uint32_t)(branch + 2));
					*log_phase_retaddr_fix =
						reset_movl_imm(
								inst_in_template(patch_code, 
									__rdtsc_template, save_return_addr),
								(uint32_t)(uintptr_t)inst_in_template(patch_code,
									__rdtsc_template, end));
					memcpy(patch_code + tmpsz,
							__real_branch_phase_template_start,
							real_branch_template_sz);
					return patch_sz;
				}
				default:
					FATAL(COMPILER, "unknown branch instruction: 0x%x 0x%x 0x%x\n",
							inst1, inst2, inst3);
			}
		}

		case 0xcc: {
			/* this is int3! we must restore eip and move the control back! */
			/* generate code: pushl $xxxxxxx; ret */
			/* this is push */
			patch_code[0] = 0x68;
			*(uint32_t*)(patch_code + 1) = (uint32_t)(branch);
			/* this is ret */
			patch_code[5] = 0xc3;
			return 6;
		}

		case 0xcd: {
			if (inst2 == 0x80) {
				template_sym(__int80_syscall_template_start);
				template_sym(__int80_syscall_template_end);
				/* this is int80 system call */
				*pexit_type = EXIT_SYSCALL;
				int patch_sz = template_sz(__int80_syscall_template);
				memcpy(patch_code, (void*)__int80_syscall_template_start,
						patch_sz);
				reset_movl_imm(patch_code, (uint32_t)(branch + 2));
				assert(patch_sz <= MAX_PATCH_SIZE);
				return patch_sz;
			} else {
				FATAL(COMPILER, "doesn't support int 0x%x\n", inst2);
			}
		}

		default:
			FATAL(COMPILER, "unknown branch instruction: 0x%x 0x%x 0x%x\n",
					inst1, inst2, inst3);
	}

	return 0;
}

static struct code_block_t *
compile_code_block(void * target, struct obj_page_head ** phead)
{
	TRACE(COMPILER, "compiling code block %p\n", target);
	
	uint8_t patch_code[MAX_PATCH_SIZE];
	void * branch_start = scan_insts(target);
	enum exit_type exit_type = EXIT_UNCOND_DIRECT;

	uint32_t * log_phase_retaddr_fix = NULL;
	int recompile_offset = 0;

#if 0
	int patch_sz;
	if (((uintptr_t)target != 0xb7fab716) &&
		((uintptr_t)target != 0xb7fab70a)) {
		patch_sz = compile_branch(patch_code,
			(uint8_t*)(branch_start), &exit_type,
			&log_phase_retaddr_fix, &recompile_offset);
	} else {
		patch_code[0] = 0xcc;
		patch_sz = compile_branch(patch_code+1,
			(uint8_t*)(branch_start), &exit_type,
			&log_phase_retaddr_fix, &recompile_offset);
		patch_sz += 1;
	}
#endif

	int patch_sz = compile_branch(patch_code,
			(uint8_t*)(branch_start), &exit_type,
			&log_phase_retaddr_fix, &recompile_offset);

	int ori_sz = ptr_length(branch_start, target);
	int head_sz = template_sz(__set_current_block_template);
	int block_sz = sizeof(struct code_block_t) +
		head_sz +
		ori_sz +
		patch_sz;
	assert(patch_sz >= SMALLEST_PATCH_SIZE);
	struct code_block_t * block = alloc_obj(phead, block_sz);
	assert(block != NULL);

	/* set code_block_t */
	block->entry = target;
	block->exit_inst_addr = branch_start;
	block->ori_code_end = &block->__code[head_sz + ori_sz];
	block->exit_type = exit_type;
	memcpy(block->__code,
			__set_current_block_template_start, head_sz);
	/* __set_current_block_template is a signle 'movl' instruction */
	reset_movl_imm(block->__code, (uint32_t)(block));
	memcpy(block->__code + head_sz,
			target, ori_sz);
	/* fix log phase insts */
	if (log_phase_retaddr_fix != NULL)
		*log_phase_retaddr_fix =
			(uintptr_t)(block->ori_code_end) +
			((uintptr_t)(*log_phase_retaddr_fix) -
			 (uintptr_t)(patch_code));
	memcpy(block->ori_code_end, patch_code, patch_sz);
	if (block->exit_type == EXIT_UNCOND_DIRECT) {
		block->recompile_start = block->ori_code_end + recompile_offset;
	} else {
		block->recompile_start = NULL;
	}
	TRACE(COMPILER, "new block %p for %p compiled, __code=%p, ori_code_end=%p\n",
			block, block->entry, block->__code, block->ori_code_end);
	return block;
}

static struct code_block_t *
get_block(struct tls_code_cache_t * cache, void * target)
{
	struct code_block_t * block = (void*)dict_get(cache->cache_dict,
			(uintptr_t)(target));
	return block;
}

static __AI void
recompile_ud(struct code_block_t * block, void * target)
{
	/* e9 is near, relative jmp */
	uint8_t * branch = block->recompile_start;
	branch[0] = 0xe9;
	*((int32_t*)(branch + 1)) = (uint32_t)(target) -
		((uint32_t)(branch) + 5);
	TRACE(COMPILER, "ud block %p (entry: %p) recompiled\n", block, block->entry);
	return;
}

static struct code_block_t *
do_compile(struct thread_private_data * tpd)
{
	/* this is the entry of compiler */
	TRACE(COMPILER, "finding %p in do_compile, esp=%p\n", tpd->target,
			tpd->old_stack_top);
	void * target = tpd->target;
	struct tls_code_cache_t * cache = &(tpd->code_cache);
	struct code_block_t * block = get_block(cache, target);

	if (block == NULL) {
		TRACE(COMPILER, "unable to find %p, real compile\n", tpd->target);
		block = compile_code_block(target, &cache->code_blocks);
		assert(block != NULL);
		dict_insert(&cache->cache_dict,
				(uintptr_t)(block->entry),
				(uintptr_t)block);
	}

	if (cache->current_block &&
		 (cache->current_block->exit_type ==
		  EXIT_UNCOND_DIRECT))
	{
		recompile_ud(cache->current_block, block->__code);
	}

	TRACE(COMPILER, "compile over, restart from %p\n", block->__code);
	return block;
}

void
do_real_branch(void)
{
	struct thread_private_data * tpd = get_tpd();
	struct tls_code_cache_t * cache = &tpd->code_cache;

#if 0
	if ((uintptr_t)tpd->target == 0x8048560)
		breakpoint();
#endif

	if (cache->current_block->entry ==
			 tpd->target)
	{
		tpd->target = cache->current_block->__code;
		return;
	}

	if (cache->fast_block1->entry ==
			 tpd->target)
	{
		tpd->target = cache->fast_block1->__code;
		return;
	}
	if (cache->fast_block2->entry ==
			 tpd->target)
	{
		tpd->target = cache->fast_block2->__code;
		struct code_block_t * tmp;
		tmp = cache->fast_block1;
		cache->fast_block1 = cache->fast_block2;
		cache->fast_block2 = tmp;
		return;
	}

	if (cache->fast_block3->entry ==
			 tpd->target)
	{
		tpd->target = cache->fast_block3->__code;
		struct code_block_t * tmp = cache->fast_block3;
		cache->fast_block3 = cache->fast_block2;
		cache->fast_block2 = cache->fast_block1;
		cache->fast_block1 = tmp;
		return;
	}

	struct code_block_t * block = do_compile(tpd);
	tpd->target = block->__code;
	cache->fast_block3 = cache->fast_block2;
	cache->fast_block2 = cache->fast_block1;
	cache->fast_block1 = block;
	return;
}

// vim:ts=4:sw=4

