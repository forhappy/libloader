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

static int
compile_branch(uint8_t patch_code[], void * branch,
		enum exit_type * pexit_type)
{
#warning unfinished
	TRACE(COMPILER, "compiling branch %p\n", branch);
	uint8_t inst = *(uint8_t*)(branch);
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

	int patch_sz = compile_branch(patch_code, branch_start, &exit_type);

	int ori_sz = (uintptr_t)(branch_start) - (uintptr_t)(target);

	int block_sz = sizeof(struct code_block_t) +
		ori_sz +
		patch_sz;
	struct code_block_t * block = alloc_obj(phead, block_sz);
	assert(block != NULL);

	block->entry = target;
	block->exit_inst_addr = branch_start;
	block->ori_code_end = (&(block->__code[ori_sz]));
	block->exit_type = exit_type;
	memcpy(block->__code, target, ori_sz);
	memcpy(&(block->__code[ori_sz]), patch_code, patch_sz);

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
	/* this is ljmp *0xxxxxxxxx */
	uint8_t * branch = block->ori_code_end;
	branch[0] = 0xff;
	branch[1] = 0x2d;
	*((void**)(branch + 2)) = target;
	return;
}

static __AI void
__compile_code_block(bool_t recompild_ud)
{
	struct thread_private_data * tpd = get_tpd();
	struct tls_code_cache_t * cache = &(tpd->code_cache);
	
	assert(tpd != NULL);
	void * target = tpd->target;
	assert(cache != NULL);

	DEBUG(COMPILER, "recompile block %p, target is %p\n",
			cache->last_ud_block, target);

	struct code_block_t * block = get_block(cache, target);
	assert(block != NULL);
	/* recompile */
	if (recompild_ud) {
		struct code_block_t * last_ud_block = cache->last_ud_block;
		if (last_ud_block != NULL) {
			assert(last_ud_block->exit_type == EXIT_UNCOND_DIRECT);
			__recompile_ud(last_ud_block, block->__code);
		}
	}

#warn how about signal???
	/* reset last_ud_block */
	if (block->exit_type == EXIT_UNCOND_DIRECT)
		cache->last_ud_block = block;
	tpd->target = block->__code;
	TRACE(COMPILER, "target address: %p\n", tpd->target);
	return;
	
}

void
recompile_ud_code_block(void)
{
	__compile_code_block(TRUE);
}

/* block entry is taken from tpd->target */
void
compile_code_block(void)
{
	__compile_code_block(FALSE);
}

// vim:ts=4:sw=4

