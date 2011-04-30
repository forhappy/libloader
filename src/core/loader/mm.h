/* 
 * interp/mm.h
 * by WN @ Oct. 25, 2010
 */

#ifndef LOADER_MM_H
#define LOADER_MM_H

#include <list.h>
#include <defs.h>


/* name clash: alloc_pages */
extern void *
xalloc_pages(size_t sz, bool_t executable);

extern void
xdealloc_pages(void * ptr, size_t sz);

/* 32 MB */
#define OBJ_BUCKET_SIZE	(32 * (1 << 20))

struct obj_buk_head {
	struct list_head list;
	void * buk_addr;
	void * buk_end;
	void * cur_ptr;
	size_t total_size;
	size_t aval_size;
};

#define MAX_AVAL_OBJ_SIZE	(OBJ_BUCKET_SIZE - sizeof(struct obj_buk_head))

struct obj_mm_tpd_stub {
	struct list_head buk_list;
	struct obj_buk_head * cur_buk;
	bool_t executable;
};

extern void
init_mm_obj(struct obj_mm_tpd_stub * obj_mm_tls, bool_t executable);

extern void
init_self_mm_obj(bool_t executable);

extern void
clean_mm_obj(struct obj_mm_tpd_stub * obj_mm_tls);

extern void
clean_self_mm_obj(void);

extern void *
alloc_obj(struct obj_mm_tpd_stub * obj_mm_tls, size_t size);

extern void *
alloc_self_obj(size_t size);

#endif

// vim:ts=4:sw=4

