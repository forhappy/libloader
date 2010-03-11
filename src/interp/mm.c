/* 
 * mm.c
 * by WN @ Mar. 10, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <asm/utils.h>

#include <interp/mm.h>

void *
alloc_cont_space(int sz)
{
	sz = ALIGN_UP(sz, PAGE_SIZE);
	int nr = sz >> PAGE_SHIFT;
	void * start = alloc_pages(nr, FALSE);
	assert(start != NULL);
	struct cont_space_header * head = start;
	head->real_sz = sz;
	return (void*)(head->__data);
}

static struct obj_page_head *
alloc_obj_pages(int sz, struct obj_page_head * next)
{
	sz = ALIGN_UP(sz + sizeof(struct obj_page_head *),
			PAGE_SIZE);
	int nr = sz >> PAGE_SHIFT;
	void * start = alloc_pages(nr, TRUE);
	assert(start != NULL);
	struct obj_page_head * head = start;
	head->space_sz = sz;
	head->next_free_byte = head->__data;
	head->next_obj_page = next;
	TRACE(MEM, "object page %p alloced, length is 0x%x, next is %p\n",
			head, sz, next);
	return head;
}

void *
alloc_obj(struct obj_page_head ** phead, int sz)
{
	assert(phead != NULL);
	TRACE(MEM, "alloc %d bytes from %p\n", sz, *phead);
	struct obj_page_head * head = *phead;
	for (; head != NULL; head = head->next_obj_page) {
		if (head->next_free_byte + sz < (void*)(head) + head->space_sz) {
			/* we find it */
			void * ptr = head->next_free_byte;
			head->next_free_byte = ptr + sz;
			TRACE(MEM, "find free space %p from alloced page\n", ptr);
			return ptr;
		}
	}
	/* we need alloc new page */
	TRACE(MEM, "unable to find enough free space\n");
	struct obj_page_head * new_head = alloc_obj_pages(sz, *phead);
	assert(new_head != NULL);
	void * ptr;
	ptr = new_head->next_free_byte;
	new_head->next_free_byte += sz;
	assert(new_head->next_free_byte < (void*)(new_head) + (new_head)->space_sz);
	*phead = new_head;
	TRACE(MEM, "find free space %p from new page\n", ptr);
	return ptr;
}

void
clear_obj_pages(struct obj_page_head ** phead)
{
	assert(phead != NULL);
	struct obj_page_head * head = *phead;
	while (head != NULL) {
		struct obj_page_head * next = head->next_obj_page;
		uint32_t space_sz = head->space_sz;
		free_pages((void*)head, space_sz >> PAGE_SHIFT);
		TRACE(MEM, "obj page %p, length 0x%x, unmapped\n",
				head, space_sz);
		head = next;
	}
	*phead = NULL;
}


// vim:ts=4:sw=4

