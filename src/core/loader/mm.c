/* 
 * x86/interp/mm.c
 * arch dependent mm
 * by WN @ Oct. 26, 2010
 */

#include <linux/mman.h>
#include <asm/page.h>

#include <defs.h>
#include <debug.h>
#include <list.h>
#include <loader/mm.h>
#include <syscall.h>

#include <loader/snitchaser_tpd.h>
#include <loader/processor.h>

void *
xalloc_pages(size_t sz, bool_t executable)
{
	assert(sz > 0);
	TRACE(MEM, "allocing 0x%lx bytes of pages\n", (long int)sz);
	sz = ALIGN_UP(sz, PAGE_SIZE);
	int prot = PROT_READ | PROT_WRITE;
	if (executable)
		prot |= PROT_EXEC;
	void * ptr = sys_uniform_mmap(NULL, sz, prot,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert(is_valid_page(ptr));
	TRACE(MEM, "got 0x%lx bytes of pages %p\n", (long int)sz, ptr);
	return ptr;
}

void
xdealloc_pages(void * ptr, size_t sz)
{
	assert(is_valid_page(ptr));
	TRACE(MEM, "unmapping 0x%lx bytes of pages: %p\n", (long int)sz, ptr);
	sz = ALIGN_UP(sz, PAGE_SIZE);
	int err = sys_munmap(ptr, sz);
	assert(err == 0);
}

static struct obj_buk_head *
alloc_obj_buk(bool_t executable)
{
	void * mem = xalloc_pages(OBJ_BUCKET_SIZE, executable);
	assert(mem != NULL);

	struct obj_buk_head * buk = mem;
	INIT_LIST_HEAD(&buk->list);
	buk->cur_ptr = (void*)(&buk[1]);

	buk->buk_addr = mem;
	buk->buk_end = mem + OBJ_BUCKET_SIZE;
	buk->total_size =
		buk->buk_end - buk->cur_ptr;
	buk->aval_size = buk->total_size;

	TRACE(MEM, "obj bucket %p allocated\n", buk);
	return buk;
}

static void
free_obj_buk(struct obj_buk_head * buk)
{
	VERBOSE(MEM, "free_obj_buk()\n");
	assert(buk != NULL);
	assert(buk->buk_addr != NULL);
	xdealloc_pages(buk->buk_addr, OBJ_BUCKET_SIZE);
}

void
init_mm_obj(struct obj_mm_tpd_stub * objmm, bool_t executable)
{
	assert(objmm != NULL);

	TRACE(MEM, "thread %d:%d init mm obj subsystem: %p\n",
			sys_getpid(), sys_gettid(), objmm);

	memset(objmm, '\0', sizeof(*objmm));
	INIT_LIST_HEAD(&objmm->buk_list);

	objmm->executable = executable;
	objmm->cur_buk = NULL;

	TRACE(MEM, "subsystem: %p is ok\n", objmm);
}

void
clean_mm_obj(struct obj_mm_tpd_stub * objmm)
{
	assert(objmm != NULL);
	VERBOSE(MEM, "cleanning objmm %p\n", objmm);

	struct obj_buk_head * buk, * n;
	list_for_each_entry_safe(buk, n, &objmm->buk_list, list) {
		list_del(&buk->list);
		free_obj_buk(buk);
	}
	memset(objmm, '\0', sizeof(*objmm));
}

static void *
alloc_from_buk(struct obj_buk_head * buk, size_t size)
{
	if (buk == NULL)
		return NULL;
	if (buk->aval_size < size)
		return NULL;

	void * ptr = buk->cur_ptr;
	buk->cur_ptr += size;
	assert(buk->cur_ptr < buk->buk_end);
	buk->aval_size -= size;
	return ptr;
}

void *
alloc_obj(struct obj_mm_tpd_stub * objmm, size_t size)
{
	assert(objmm != NULL);
	if (size >= MAX_AVAL_OBJ_SIZE) {
		FATAL(MEM, "allocing object (%ld bytes) too large (limitation is %ld)\n",
				(long int)size, (long int)MAX_AVAL_OBJ_SIZE);
	}
	assert(size > 0);

	TRACE(MEM, "allocing %ld bytes obj from %p\n",
			(long int)size, objmm);

	/* try current buk */
	struct obj_buk_head * cur_buk = objmm->cur_buk;

	void * ptr = alloc_from_buk(cur_buk, size);
	if (ptr != NULL) {
		TRACE(MEM, "obj allocated from fast path %p\n", ptr);
		return ptr;
	}

	/* find the biggest available buk */
	struct obj_buk_head * buk, * max_buk = NULL;
	size_t max_aval = 0;
	list_for_each_entry(buk, &objmm->buk_list, list) {
		if (buk->aval_size > max_aval) {
			max_buk = buk;
			max_aval = buk->aval_size;
		}
	}

	TRACE(MEM, "max_buk is %p (%ld)\n", max_buk, (long int)max_aval);
	if ((max_buk != NULL) &&
			(max_buk != cur_buk) &&
			(max_buk->aval_size > size)) {
		void * ptr = alloc_from_buk(max_buk, size);
		assert(ptr != NULL);

		TRACE(MEM, "got in max_buk, return %p\n", ptr);
		/* reset curr buk */
		if (max_buk->aval_size > cur_buk->aval_size) {
			objmm->cur_buk = max_buk;
			TRACE(MEM, "cur_buk switched\n");
		}
		return ptr;
	}

	/* alloc new buk */
	struct obj_buk_head * new_buk =
		alloc_obj_buk(objmm->executable);
	assert(new_buk != NULL);
	/* link */
	list_add(&new_buk->list, &objmm->buk_list);

	ptr = alloc_from_buk(new_buk, size);
	assert(ptr != NULL);
	TRACE(MEM, "got in new buk %p: %p\n",
			new_buk, ptr);

	if ((max_buk == NULL) ||
			(new_buk->aval_size > max_buk->aval_size))
		max_buk = new_buk;
	TRACE(MEM, "new max_buk is %p (%ld)\n", max_buk,
			(long int)(max_buk->aval_size));
	assert(max_buk != NULL);
	/* adjust curr buk */
	objmm->cur_buk = max_buk;
	TRACE(MEM, "return %p\n", ptr);
	return ptr;
}

void *
alloc_self_obj(size_t size)
{
	struct thread_private_data * tpd = get_self_tpd();
	assert(tpd != NULL);
	struct obj_mm_tpd_stub * p = &tpd->mm_stub;
	return alloc_obj(p, size);
}

void
init_self_mm_obj(bool_t executable)
{
	struct thread_private_data * tpd = get_self_tpd();
	assert(tpd != NULL);
	struct obj_mm_tpd_stub * p = &tpd->mm_stub;
	init_mm_obj(p, executable);
}

void
clean_self_mm_obj(void)
{
	struct thread_private_data * tpd = get_self_tpd();
	assert(tpd != NULL);
	struct obj_mm_tpd_stub * p = &tpd->mm_stub;
	clean_mm_obj(p);
}

// vim:ts=4:sw=4

