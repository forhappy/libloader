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
#warning not implemented
	TRACE(COMPILER, "compiling branch %p\n", branch);
	return 0;
}

static struct code_block_t *
compile_code(void * target, struct obj_page_head ** phead,
		struct dict_t ** pdict)
{
#warning not implemented

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
	return NULL;
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

	block = compile_code(target, &cache->code_blocks,
			&cache->cache_dict);
	assert(block != NULL);
#warning not implemented
	return block;
}

void
recompile_ud_code_block(void)
{
#warning Not implemented
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
	/* reset last_ud_block */
	if (block->exit_type == EXIT_UNCOND_DIRECT)
		cache->last_ud_block = block;
	tpd->target = block->__code;
	return;
}

void
compiler_entry(void)
{
#warning Not implemented
}

// vim:ts=4:sw=4

