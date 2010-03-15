/* 
 * compiler.c
 * by WN @ Mar. 16, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>
#include <interp/mm.h>
#include <interp/code_cache.h>
#include <asm/compiler.h>
#include <asm/tls.h>
#include <asm/debug.h>

struct code_block_t *
compile_code_block(void * stream)
{
	void * branch_inst = scan_insts(stream);
	assert(branch_inst != NULL);
	return NULL;
}

void
compiler_entry(void)
{
	breakpoint();
	struct thread_private_data * tpd = get_tpd();
	TRACE(COMPILER, "comes to compiler_entry, target is 0x%x\n",
			tpd->code_cache.target);
	__exit(0);
}

// vim:ts=4:sw=4

