/* 
 * compiler.c
 * by WN @ Mar. 16, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/mm.h>
#include <common/code_cache.h>
#include <common/assert.h>
#include <asm/compiler.h>
#include <asm/tls.h>

struct code_block_t *
compile_code_block(void * stream)
{
	void * branch_inst = scan_insts(stream);
	assert(branch_inst != NULL);
	return NULL;
}

// vim:ts=4:sw=4

