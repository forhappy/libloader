/* 
 * code_cache.c
 * by WN @ Mar. 12, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <interp/code_cache.h>
#include <interp/mm.h>
#include <interp/dict.h>
#include <xasm/tls.h>
#include <xasm/logger.h>

void
clear_code_cache(struct tls_code_cache_t * cc)
{
	destroy_dict(&(cc->cache_dict));
	clear_obj_pages(&(cc->code_blocks));
}

/* 
 * make sure the current_block not NULL, we can omit
 * an NULL check
 */
static struct code_block_t first_fake_code_block = {
	.entry = NULL,
	.exit_inst_addr = NULL,
	.ori_code_end = NULL,
	.last_target_entry = NULL,
	.last_target_code = first_fake_code_block.__code,
	.exit_type = EXIT_SYSCALL,
};

/* the dict and obj pages are reset to NULL when
 * init tls */
void
init_code_cache(struct tls_code_cache_t * cc)
{
	cc->current_block = &first_fake_code_block;
}

// vim:ts=4:sw=4

