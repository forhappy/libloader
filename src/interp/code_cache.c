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

/* the dict and obj pages are reset to NULL when
 * init tls */
void
init_code_cache(void)
{
	/* nothing todo */
}

// vim:ts=4:sw=4

