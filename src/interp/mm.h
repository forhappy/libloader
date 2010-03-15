/* 
 * mm.h
 * by WN @ Mar. 08, 2010
 */

#ifndef __SNITCHASER_MM_H
#define __SNITCHASER_MM_H

#include <config.h>
#include <common/defs.h>
/* for page size */
#include <asm/utils.h>

/* 
 * the usage of mm:
 * mm is used to support code cache.
 * code cache contains 2 parts: a dict and a compiled code storage space.
 * dict should reside in a continuous memory space; code pieces can
 * use slob like mm.
 *
 * Although it is cache, code cache never shrink. If we are unable to find
 * free space for new code pieces (the code cache size exceeds a user defined
 * threshold, not implement now), we unmap all codecache and rebuild it.
 *
 * all memory of code cache of a thread is monopolistic, not share with other
 * threads.
 */

struct cont_space_header {
	uint32_t real_sz;
	uint8_t __data[];
};

/* alloc continous memory space for dict use.
 * alloc_cont_space alloc some pages according to size.
 * in x86, the return ptr should be 0xaaaab004, the 4 bytes
 * at the head of the page shows the length of actually alloced
 * size.
 * alloc_cont_space always alloc non-executable pages;
 * */
extern void *
alloc_cont_space(int size);

extern void
free_cont_space(void * ptr);

struct obj_page_head {
	uint32_t space_sz;
	void * next_free_byte;
	struct obj_page_head * next_obj_page;
	uint8_t __data[];
};

/* 
 * object pages always executable
 * */

/* 
 * phead is the original obj_page_head,
 * *phead can be NULL.
 * after we alloc an object, *phead is updated.
 */
extern void *
alloc_obj(struct obj_page_head ** phead, int sz);

extern void
clear_obj_pages(struct obj_page_head ** phead);

/* no free_obj, we don't need it! */

#endif

// vim:ts=4:sw=4

