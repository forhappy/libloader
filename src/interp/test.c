/* 
 * test.c
 * by WN @ Mar. 10, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>

#include <xasm/tls.h>
#include <xasm/string.h>

#include <interp/mm.h>
#include <interp/dict.h>
#include <interp/compress.h>

void ATTR_EXPORT
test_debug(void)
{
	VERBOSE(SYSTEM, "this is exported func, for test use only\n");
	return;
}

void ATTR_EXPORT
test_tls(void)
{
	VERBOSE(SYSTEM, "--- test thread local storage\n");
	init_tls();
	struct thread_private_data * tpd = get_tpd();
	VERBOSE(SYSTEM, "tpd reside on %p\n", tpd);
	VERBOSE(SYSTEM, "pid=%d, tid=%d, tnr=%d\n", tpd->pid, tpd->tid, tpd->tnr);
	clear_tls();
}

void ATTR_EXPORT
test_mm(void)
{
	VERBOSE(SYSTEM, "--- allocing continuous memory spaces\n");
	void * cont_space = alloc_cont_space(12345);
	memset(cont_space, '\0', 12345);
	free_cont_space(cont_space);

	VERBOSE(SYSTEM, "--- allocing objects\n");
	struct obj_page_head * obj_pages = NULL;
	for (int i = 0; i < 1000; i++) {
		void * ptr = alloc_obj(&obj_pages, i * 10);
		memset(ptr, '\0', i * 10);
	}
	clear_obj_pages(&obj_pages);
	VERBOSE(SYSTEM, "obj_pages become %p\n", obj_pages);

}

void ATTR_EXPORT
test_dict(void)
{
	VERBOSE(SYSTEM, "--- testing dict\n");
	struct dict_t * dict = NULL;
	for (int i = 0; i < 100; i++) {
		dict_insert(&dict, 0x80048000 + i * 123, i);
	}
	destroy_dict(&dict);
}

void ATTR_EXPORT
test_compression(void)
{
	VERBOSE(SYSTEM, "--- testing compression\n");
	init_tls();

	static const unsigned char buffer[] = "qwerqwerqwerqwerqwerqwerqwerqwerqwerqwer";
	static unsigned char buffer2[sizeof(buffer)];
	static unsigned char buffer3[sizeof(buffer)];
	const uint8_t * out_buf = NULL;
	int compressed_sz = 0;
	compress(buffer, sizeof(buffer), &out_buf, &compressed_sz);

	memcpy(buffer2, out_buf, compressed_sz);

	clear_tls();

	int uncompressed_sz = sizeof(buffer);
	decompress(buffer2, compressed_sz, buffer3,
			&uncompressed_sz);
	VERBOSE(SYSTEM, "uncompressed data: %s||sz=%d\n", buffer3, uncompressed_sz);
	assert(uncompressed_sz == sizeof(buffer));
}

// vim:ts=4:sw=4

