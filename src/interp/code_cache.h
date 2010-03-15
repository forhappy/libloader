/* 
 * code_cache.h
 * by WN @ Mar. 12, 2010
 */

#ifndef __CODE_CACHE_H
#define __CODE_CACHE_H

#include <config.h>
#include <common/defs.h>
#include <common/list.h>
#include <interp/dict.h>

#include <stdint.h>

enum jmp_type {
	UNCONDITIONAL,
	CONDITIONAL,
};

enum jmp_target_type {
	DIRECT,
	INDIRECT,
};

struct block_exit_target_t {
	enum jmp_type type;
	enum jmp_target_type target_type;
	uintptr_t exit_inst;
	union {
		uintptr_t taken_indirect_target_addr;
		uintptr_t taken_target;
	} taken;
	/* how about 'ret'? */
	uintptr_t untaken_target;
	/* when compile, untaken_addr_in_cache
	 * points to compiled code's untaken target address.
	 * after such codeblock compiled, change this
	 * address in cache can avoid code to transfer to
	 * compiler. */
	uintptr_t * untaken_addr_in_cache;
};

struct code_block_t {
	uintptr_t entry;
	/* block can be linked together to
	 * form a LRU list */
	struct list_head lru_list;
	/* the dict entry this block blongs to */
	struct dict_entry_t * dict_entry;
	struct block_exit_target_t exit;
	uint8_t __code[];
};

#endif

// vim:ts=4:sw=4

