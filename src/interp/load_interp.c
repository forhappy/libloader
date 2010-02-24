/* 
 * load_interp.c
 * by WN @ Feb. 24, 2010
 *
 * load the real ld-linux.so
 */

#include <common/debug.h>
#include <common/assert.h>
#include <alloca.h>
#include <asm/string.h>
#include <asm/utils.h>
#include <asm/syscall.h>
#include <asm/elf.h>

#include <sys/mman.h>

#define INTERP_FILE		"/lib/ld-linux.so.2"

static int
open_real_interp(struct elf32_hdr * hdr)
{
	int fd = INTERNAL_SYSCALL_int80(open, 2,
			INTERP_FILE, O_RDONLY);
	assert(fd >= 0);

	int err = INTERNAL_SYSCALL_int80(read, 3,
			fd, hdr, sizeof(*hdr));

	assert(err == sizeof(*hdr));
	assert(memcmp(hdr->e_ident, ELFMAG, SELFMAG) == 0);
	TRACE(LOADER, "magic check ok\n");

	/* parse header */
	assert(hdr->e_type == ET_DYN);
	assert(hdr->e_machine == EM_386);
	return fd;
}

static void
load_phdrs(int fd, struct elf32_phdr * dest, struct elf32_hdr * h)
{
	int err;
	err = INTERNAL_SYSCALL_int80(lseek, 3,
			fd, h->e_phoff, SEEK_SET);
	assert(err >= 0);
	err = INTERNAL_SYSCALL_int80(read, 3,
			fd, dest, sizeof(*dest) * h->e_phnum);
	assert(err == sizeof(*dest) * h->e_phnum);
}

static int
compute_total_map_sz(struct elf32_phdr * phdrs, int nr_phdrs,
		int * pstart, int * pend, int * nr_first)
{
	uint32_t start = 0xffffffff;
	uint32_t end = 0;
	for (int i = 0; i < nr_phdrs; i++) {
		struct elf32_phdr * p = &phdrs[i];
		if (p->p_type != PT_LOAD)
			continue;
		if (start > p->p_vaddr) {
			start = p->p_vaddr;
			*nr_first = i;
		}
		if (end < p->p_vaddr + p->p_memsz)
			end = p->p_vaddr + p->p_memsz;
		assert(p->p_align == 0x1000);
	}
	assert(end - start > 0);
	assert(end - start < 0xc0000000);

	*pstart = start;
	*pend = end;
	return end - start;
}

/* make executable section writable */
#define get_map_prot(__f) ({			\
		uint32_t __map_prot = 0;		\
		if ((__f) & PF_R)				\
			__map_prot |= PROT_READ;	\
		if ((__f) & PF_W)				\
			__map_prot |= PROT_WRITE;	\
		if ((__f) & PF_X)				\
			__map_prot |= (PROT_EXEC | PROT_WRITE);	\
		__map_prot;						\
		})

void * real_interp_address = NULL;

void *
load_interp(void * oldesp)
{
	TRACE(LOADER, "loading " INTERP_FILE "\n");

	struct elf32_hdr hdr;
	int fd = open_real_interp(&hdr);

	int nr_phdrs = hdr.e_phnum;
	struct elf32_phdr * phdrs = alloca(sizeof(struct elf32_phdr) *
			nr_phdrs);
	assert(phdrs != NULL);
	load_phdrs(fd, phdrs, &hdr);

	int start, end, nr_first;
	int total_map_sz = compute_total_map_sz(phdrs, nr_phdrs,
			&start, &end, &nr_first);
	TRACE(LOADER, "total mem sz of " INTERP_FILE " = 0x%x: 0x%x -- 0x%x, %d\n",
			total_map_sz, start, end, nr_first);

	/* map the file */
	void * map_addr;
	/* map the first section with total_map_sz, and 
	 * unmap unneed pages. This is to make sure we have a
	 * continuous memory space to hold the whole mem image. */
	uint32_t first_prot = get_map_prot(phdrs[nr_first].p_flags);
	map_addr = (void*)INTERNAL_SYSCALL_int80(mmap2, 6,
			NULL, total_map_sz, first_prot, MAP_PRIVATE,
			fd, phdrs[nr_first].p_offset >> 12);
	assert(map_addr < (void*)0xc0000000);
	TRACE(LOADER, "map from %p to %p\n",
			map_addr, map_addr + total_map_sz);

	/* unmap the padding pages */
	void * unmap_start = map_addr + phdrs[nr_first].p_memsz;
	void * unmap_end = map_addr + total_map_sz;
	unmap_start = ALIGN_UP_PTR(unmap_start, 4096);
	unmap_end = ALIGN_UP_PTR(unmap_end, 4096);
	TRACE(LOADER, "unmap from %p to %p\n",
			unmap_start, unmap_end);
	INTERNAL_SYSCALL_int80(munmap, 2,
			unmap_start, unmap_end - unmap_start);

	/* map other sections */
	for (int i = 0; i < nr_phdrs; i++) {
		if (i == nr_first)
			continue;
		if (phdrs[i].p_type != PT_LOAD)
			continue;
		void * start_addr = map_addr + phdrs[i].p_vaddr;
		void * fend_addr = start_addr + phdrs[i].p_filesz;
		void * real_fend_addr = fend_addr;
		void * mend_addr = start_addr + phdrs[i].p_memsz;
		start_addr = ALIGN_DOWN_PTR(start_addr, 4096);
		fend_addr = ALIGN_UP_PTR(fend_addr, 4096);
		mend_addr = ALIGN_UP_PTR(mend_addr, 4096);
		uint32_t prot = get_map_prot(phdrs[i].p_flags);

		uint32_t err = INTERNAL_SYSCALL_int80(mmap2, 6,
				start_addr, fend_addr - start_addr,
				prot, MAP_PRIVATE | MAP_FIXED,
				fd, (phdrs[i].p_offset) >> 12);
		TRACE(LOADER, "map file at 0x%x\n", err);
		assert(err == (uint32_t)start_addr);

		/* map additional pages */
		if (mend_addr > fend_addr) {
			/* we need more pages */
			err = INTERNAL_SYSCALL_int80(mmap2, 6,
					fend_addr, mend_addr - fend_addr,
					prot, MAP_PRIVATE | MAP_FIXED,
					0, 0);
			assert(err == (uint32_t)fend_addr);
			TRACE(LOADER, "pad at 0x%x\n", err);
		}
		/* cleanup */
		memset(real_fend_addr,
				'\0', mend_addr - real_fend_addr);
	}
	INTERNAL_SYSCALL_int80(close, 1,
			fd);
	real_interp_address = map_addr;
	return map_addr + hdr.e_entry;
}

// vim:ts=4:sw=4

