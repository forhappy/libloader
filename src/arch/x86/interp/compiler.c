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
#include <asm/compiler.h>
#include <asm/tls.h>
#include <asm/debug.h>
#include <asm/string.h>

#define MAX_PATCH_SIZE	(256)

static __AI void
reset_movl_imm(uint8_t * inst, uint32_t data)
{
	if ((inst[0] == 0x64) && (inst[1] == 0xc7) && (inst[2] == 0x05)) {
		/* this is movl $0xffffffff, %fs:0x12345678 */
		*((uint32_t*)(inst + 7)) = data;
	} else if ((inst[0] == 0xc7) && (inst[1] == 0x05)) {
		/* this is movl $0xffffffff, 0x12345678 */
		*((uint32_t*)(inst + 6)) = data;
	}
}

#define template_sym(s)		extern ATTR_HIDDEN uint8_t n[]

template_sym(__set_current_block_template_start);
template_sym(__set_current_block_template_end);
template_sym(__log_phase_template_start);
template_sym(__log_phase_template_end);
template_sym(__log_phase_template_commit);
template_sym(__log_phase_template_return_ebx);
template_sym(__log_phase_template_return_addr);

void
do_real_branch(void)
{
	/* this is the entry of compiler */
	FATAL(COMPILER, "XXXX\n");
	return;
}

void
do_recompile_ud_branch(void)
{
	FATAL(COMPILER, "YYYY\n");
	return;
}

#if 0
extern uint8_t __set_current_block_template_start[];
extern uint8_t __set_current_block_template_end[];
extern uint8_t __set_current_block_template_movl[];
extern uint8_t __set_current_block_template_before_set_ccb[];
extern uint8_t __set_current_block_template_before_restore_eax[];


static int
compile_branch(uint8_t patch_code[], uint8_t * branch,
		enum exit_type * pexit_type)
{
#warning unfinished
	TRACE(COMPILER, "compiling branch %p\n", branch);
	uint8_t inst = *branch;
	switch (inst) {
#define COMP_Jxx(x, n)	\
		case x:	{		\
			extern uint8_t __##n##_template_start[] ATTR_HIDDEN;	\
			extern uint8_t __##n##_template_end[] ATTR_HIDDEN;		\
			extern uint8_t __##n##_template_taken_movl[] ATTR_HIDDEN;	\
			extern uint8_t __##n##_template_untaken_movl[] ATTR_HIDDEN;	\
			*pexit_type = EXIT_COND_DIRECT;							\
			void * untaken_exit = branch + 2;						\
			void * taken_exit = branch + 2 + *((int8_t*)(branch + 1));	\
			int patch_sz = __##n##_template_end - __##n##_template_start;	\
			assert(patch_sz < MAX_PATCH_SIZE);\
			memcpy(patch_code, (void*)(__##n##_template_start), patch_sz);	\
			uintptr_t * puntaken_addr = (uintptr_t*)&patch_code[__##n##_template_untaken_movl - \
				__##n##_template_start + 1];				\
			uintptr_t * ptaken_addr = (uintptr_t*)&patch_code[__##n##_template_taken_movl - \
				__##n##_template_start + 1];				\
			*puntaken_addr = (uintptr_t)(untaken_exit);		\
			*ptaken_addr = (uintptr_t)(taken_exit);			\
			return patch_sz;								\
		}
		COMP_Jxx(0x70, jo)
		COMP_Jxx(0x71, jno)
		COMP_Jxx(0x72, jb)
		COMP_Jxx(0x73, jnb)
		COMP_Jxx(0x74, jz)
		COMP_Jxx(0x75, jnz)
		COMP_Jxx(0x76, jna)
		COMP_Jxx(0x77, ja)
#undef COMP_Jxx

		case 0xcd: {
			/* this is 'int' */
			if (branch[1] == 0x80) {
				/* this is system call */
			} else {
				FATAL(COMPILER, "doesn't support int 0x%x\n", branch[1]);
			}
		}
		default:
			FATAL(COMPILER, "unknown branch inst: 0x%x\n", inst);
	}
	return 0;
}

static struct code_block_t *
do_compile(void * target, struct obj_page_head ** phead,
		struct dict_t ** pdict)
{
	TRACE(COMPILER, "compiling code block from %p\n", target);

	uint8_t patch_code[MAX_PATCH_SIZE];
	void * branch_start = scan_insts(target);
	enum exit_type exit_type;

	int patch_sz = compile_branch(patch_code,
			(uint8_t*)(branch_start), &exit_type);

	int ori_sz = (uintptr_t)(branch_start) - (uintptr_t)(target);
	int head_sz = (uintptr_t)(__set_current_block_template_end) -
		(uintptr_t)(__set_current_block_template_start);

	int block_sz = sizeof(struct code_block_t) +
		head_sz +
		ori_sz +
		patch_sz;
	struct code_block_t * block = alloc_obj(phead, block_sz);
	assert(block != NULL);

	block->entry = target;
	block->exit_inst_addr = branch_start;
	block->ori_code_end = (&(block->__code[head_sz + ori_sz]));
	block->exit_type = exit_type;
	memcpy(block->__code,
			__set_current_block_template_start, head_sz);
	reset_movl(block->__code +
				(__set_current_block_template_movl -
				 __set_current_block_template_start), (uintptr_t)(block));
	memcpy(block->__code + head_sz,
			target, ori_sz);
	memcpy(block->__code + head_sz + ori_sz,
			patch_code, patch_sz);

	dict_insert(pdict, (uintptr_t)(target), (uintptr_t)(block));
	return block;
}



static struct code_block_t *
get_block(struct tls_code_cache_t * cache, void * target)
{
	/* try to fetch from cache */
	struct code_block_t * block = (void*)dict_get(cache->cache_dict,
			(uintptr_t)(target));
	if (block != NULL) {
		DEBUG(COMPILER, "find block %p for target %p\n", block, target);
		return block;
	}

	/* not in cache */
	block = do_compile(target, &cache->code_blocks,
			&cache->cache_dict);
	assert(block != NULL);
	return block;
}

static __AI void
__recompile_ud(struct code_block_t * block, void * target)
{
#warning need test
	/* this is jmpl *0xxxxxxxxx */
	uint8_t * branch = block->ori_code_end;
	branch[0] = 0xff;
	branch[1] = 0x25;
	*((void**)(branch + 2)) = target;
	return;
}

/* block entry is taken from tpd->target */
void
compile_code_block(void)
{
	struct thread_private_data * tpd = get_tpd();
	struct tls_code_cache_t * cache = &(tpd->code_cache);
	
	assert(tpd != NULL);
	void * target = tpd->target;
	assert(cache != NULL);

	struct code_block_t * block = get_block(cache, target);
	assert(block != NULL);
	/* recompile */
	if (cache->current_block &&
			(cache->current_block->exit_type == 
			 EXIT_UNCOND_DIRECT))
	{
		__recompile_ud(cache->current_block, block->__code);
	}
	tpd->target = block->__code;
	TRACE(COMPILER, "target address: %p\n", tpd->target);
	return;
	
}
#endif

// vim:ts=4:sw=4

