/* 
 * code_cache.c
 * by WN @ Mar. 12, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <interp/code_cache.h>
#include <interp/mm.h>
#include <interp/dict.h>
#include <asm/tls.h>
#include <asm/compiler.h>

void
clear_code_cache(struct tls_code_cache_t * cc)
{
	destroy_dict(&(cc->cache_dict));
	clear_obj_pages(&(cc->code_blocks));
}

/* 
 * the only work we need to do is to
 * set compiler_entry to TLS
 */
/* the dict and obj pages are reset to NULL when
 * init tls */
void
init_code_cache(void)
{
	struct thread_private_data * tpd = get_tpd();
	tpd->compiler_entry = compiler_entry;
}

// vim:ts=4:sw=4

