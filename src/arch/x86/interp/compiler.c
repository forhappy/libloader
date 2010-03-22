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

#undef COMP_Jxx_8b
#undef CASE_Jxx_8b
#undef COMP_Jxx_8bitrel
#undef COMP_Jxx

		case 0xeb: {
			template_sym(__direct_jmp_template_start);
			template_sym(__direct_jmp_template_end);
			/* this is short jmp */
			*pexit_type = EXIT_UNCOND_DIRECT;
			int patch_sz = template_sz(__direct_jmp_template) +
				real_branch_template_sz;
			memcpy(patch_code, __direct_jmp_template_start,
					template_sz(__direct_jmp_template));
			void * target = branch + 2 + *((int8_t*)(branch + 1));
			reset_movl_imm(patch_code, (uint32_t)(target));
			/* no log phase */
			memcpy(patch_code + template_sz(__direct_jmp_template),
					__real_branch_phase_template_start,
					real_branch_template_sz);
			*recompile_branch_offset = 0;
			return patch_sz;
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
	enum exit_type exit_type;

	uint32_t * log_phase_retaddr_fix = NULL;
	int recompile_offset = 0;
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
	TRACE(COMPILER, "ud block %p recompiled\n", block);
	return;
}

static void
do_compile(struct thread_private_data * tpd)
{
	/* this is the entry of compiler */
	TRACE(COMPILER, "branch: target=%p\n", tpd->target);
	void * target = tpd->target;
	struct tls_code_cache_t * cache = &(tpd->code_cache);
	struct code_block_t * block = get_block(cache, target);

	if (block == NULL) {
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

	tpd->target = block->__code;
	TRACE(COMPILER, "target address: %p\n", tpd->target);
	return;
}

void
do_real_branch(void)
{
	struct thread_private_data * tpd = get_tpd();
	if (tpd->code_cache.current_block->entry ==
			 tpd->target) {
		tpd->target = tpd->code_cache.current_block->__code;
	} else {
		do_compile(tpd);
	}
}

// vim:ts=4:sw=4

