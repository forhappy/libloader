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
#include <asm/tls.h>
#include <asm/logger.h>

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
	tpd->logger_entry = logger_entry;
	/* 4 fast logger entry */
	tpd->ud_logger_entry = ud_logger_entry;
	tpd->ui_logger_entry = ui_logger_entry;
	tpd->cd_logger_entry = cd_logger_entry;
	tpd->ci_logger_entry = ci_logger_entry;
}

/* called by tpd->logger_entry */
void
heavy_logger_entry(void)
{
	TRACE(LOGGER, "come into heavy_logger_entry\n");
#warning add buffer size and buffer into TLS !!!
}

// vim:ts=4:sw=4

