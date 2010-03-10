/* 
 * asm/relocate.c
 * by WN @ Feb. 23, 2010
 */

#include <stdint.h>
#include <common/defs.h>
#include <common/assert.h>
#include <common/debug.h>

#include <asm/syscall.h>
#include <asm/utils.h>
#include <asm/elf.h>
#include <asm/string.h>

#include <sys/mman.h>

/* see glibc code: sysdeps/i386/elf/configure.in: 
 * 
 * dnl It is always possible to access static and hidden symbols in an
 * dnl position independent way.
 * AC_DEFINE(PI_STATIC_AND_HIDDEN)
 *
 * also see glibc's dl-machine.h
 *
 * %ebx is _GLOBAL_OFFSET_TABLE_
 * _GLOBAL_OFFSET_TABLE_[0] is link time address _DYNAMIC;
 * */

extern struct elf32_dyn _DYNAMIC[] ATTR_HIDDEN;
extern void* _GLOBAL_OFFSET_TABLE_[] ATTR_HIDDEN;

void
relocate_interp(void)
{
	uint32_t load_bias = (void*)(_DYNAMIC) - _GLOBAL_OFFSET_TABLE_[0];

	/* prepare relocation */
	struct elf32_rel * reltab = NULL;
	struct elf32_sym * symtab = NULL;
	int relsz = 0;
	int relent = 0;
	for (struct elf32_dyn * dyn = &_DYNAMIC[0]; dyn->d_tag != DT_NULL; dyn ++) {
		switch (dyn->d_tag) {
			case DT_RELSZ:
				relsz = dyn->d_un.d_val;
				break;
			case DT_REL:
				reltab = (void*)dyn->d_un.d_ptr + load_bias;
				break;
			case DT_RELENT:
				relent = dyn->d_un.d_val;
				break;
			case DT_SYMTAB:
				symtab = (void*)dyn->d_un.d_ptr + load_bias;
				break;
#if 0
			case DT_PLTGOT:
			case DT_PLTRELSZ:
			case DT_PLTREL:
			case DT_JMPREL:
#endif
		}
	}
	
	assert(reltab != NULL);
	assert(relsz != 0);
	assert(relent != 0);
	int nr_rels = relsz / relent;

	for (int i = 0; i < nr_rels; i++) {
		uint32_t address;
		uint32_t info;

		address = reltab[i].r_offset + load_bias;
		info = reltab[i].r_info;

		int type = ELF32_R_TYPE(info);

		uint32_t real_val;
		switch (type) {
			case R_386_RELATIVE: {
				uint32_t old_val = *(uint32_t*)(address);
				real_val = old_val + load_bias;
				break;
			}
			case R_386_GLOB_DAT: {
				assert(symtab != NULL);
				int nsym = ELF32_R_SYM(info);
				struct elf32_sym * sym = &symtab[nsym];
				uint32_t sym_val = sym->st_value;
				real_val = sym_val + load_bias;
				break;
			}
			default: {
				/* printf may be unusable now */
				/* printf("relocate error: unknown relocate type 0x%x\n", type); */
				__exit(-1);
			}
		}
		*(uint32_t*)(address) = real_val;
	}
}

static int
open_elf_file(struct elf32_hdr * hdr, const char * fn)
{
	int fd = INTERNAL_SYSCALL_int80(open, 2,
			fn, O_RDONLY);
	if (fd < 0)
		FATAL(LOADER, "open file %s failed: %d\n", fn, fd);

	int err = INTERNAL_SYSCALL_int80(read, 3,
			fd, hdr, sizeof(*hdr));

	assert(err == sizeof(*hdr));
	assert(memcmp(hdr->e_ident, ELFMAG, SELFMAG) == 0);
	TRACE(LOADER, "magic check ok\n");

	/* parse header */
	assert((hdr->e_type == ET_DYN) || (hdr->e_type == ET_EXEC));
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
	assert(err == (int)(sizeof(*dest) * h->e_phnum));
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

#define get_map_prot(__f) ({			\
		uint32_t __map_prot = 0;		\
		if ((__f) & PF_R)				\
			__map_prot |= PROT_READ;	\
		if ((__f) & PF_W)				\
			__map_prot |= PROT_WRITE;	\
		if ((__f) & PF_X)				\
			__map_prot |= PROT_EXEC;	\
		__map_prot;						\
		})

void *
load_elf(const char * fn, void ** p_load_bias,
		struct elf32_phdr ** ppuser_phdrs, int * p_nr_user_phdrs)
{
	assert(fn != NULL);
	TRACE(LOADER, "loading %s\n", fn);

	struct elf32_hdr hdr;
	int fd = open_elf_file(&hdr, fn);

	int nr_phdrs = hdr.e_phnum;
	struct elf32_phdr * phdrs = alloca(sizeof(struct elf32_phdr) *
			nr_phdrs);
	assert(phdrs != NULL);
	load_phdrs(fd, phdrs, &hdr);

	int start = 0, end = 0, nr_first = 0;
	int total_map_sz = compute_total_map_sz(phdrs, nr_phdrs,
			&start, &end, &nr_first);
	TRACE(LOADER, "total mem sz of %s = 0x%x: 0x%x -- 0x%x, %d\n",
			fn, total_map_sz, start, end, nr_first);

	/* map the file */
	void * map_addr = NULL;
	void * end_addr = NULL;
	void * load_bias = NULL;
	uint32_t map_type = MAP_PRIVATE;
	
	map_addr = (void*)phdrs[nr_first].p_vaddr;
	end_addr = map_addr + total_map_sz;
	map_addr = ALIGN_DOWN_PTR(map_addr, 4096);
	total_map_sz = ALIGN_UP_PTR(end_addr, 4096) - map_addr;

	if (hdr.e_type == ET_EXEC) {
		map_type |= MAP_FIXED;
	} else {
		assert(map_addr == NULL);
	}

	/* map the first segment with total_map_sz, and 
	 * unmap unneed pages. This is to make sure we have enough
	 * continuous memory space to hold the whole mem image. */
	assert(phdrs[nr_first].p_offset == 0);
	uint32_t first_prot = get_map_prot(phdrs[nr_first].p_flags);
	map_addr = (void*)INTERNAL_SYSCALL_int80(mmap2, 6,
			map_addr, total_map_sz, first_prot, map_type,
			fd, phdrs[nr_first].p_offset >> 12);
	assert(map_addr < (void*)0xc0000000);
	TRACE(LOADER, "map from %p to %p\n",
			map_addr, map_addr + total_map_sz);

	if (hdr.e_type == ET_EXEC) {
		load_bias = NULL;
	} else {
		load_bias = map_addr;
	}

	/* unmap the padding pages */
	void * unmap_start = load_bias + phdrs[nr_first].p_vaddr + phdrs[nr_first].p_memsz;
	void * unmap_end = load_bias + (uint32_t)(end_addr);

	/* pad first physical region */
	if ((phdrs[nr_first].p_filesz < phdrs[nr_first].p_memsz) &&
			(phdrs[nr_first].p_flags & PF_W))
	{
		memset(load_bias + phdrs[nr_first].p_filesz,
				'\0',
				phdrs[nr_first].p_memsz - phdrs[nr_first].p_filesz);
	}

	unmap_start = ALIGN_UP_PTR(unmap_start, 4096);
	unmap_end = ALIGN_UP_PTR(unmap_end, 4096);
	TRACE(LOADER, "unmap from %p to %p\n",
			unmap_start, unmap_end);
	INTERNAL_SYSCALL_int80(munmap, 2,
			unmap_start, unmap_end - unmap_start);

	/* map other segments */
	for (int i = 0; i < nr_phdrs; i++) {
		if (i == nr_first)
			continue;
		if (phdrs[i].p_type != PT_LOAD)
			continue;
		void * start_addr = load_bias + phdrs[i].p_vaddr;
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
		TRACE(LOADER, "map file from 0x%x to %p\n", err, fend_addr);
		assert(err == (uint32_t)start_addr);

		/* map additional pages */
		if (mend_addr > fend_addr) {
			/* we need more pages */
			TRACE(LOADER, "mapping zero page from %p to %p\n",
					fend_addr, mend_addr);
			err = INTERNAL_SYSCALL_int80(mmap2, 6,
					fend_addr, mend_addr - fend_addr,
					prot, MAP_PRIVATE | MAP_ANONYMOUS | MAP_DENYWRITE | MAP_FIXED,
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
	if (p_load_bias)
		*p_load_bias = load_bias;
	/* e_phoff is offset in file. don't use load_bias */
	if (ppuser_phdrs)
		*ppuser_phdrs = map_addr + hdr.e_phoff;
	if (p_nr_user_phdrs)
		*p_nr_user_phdrs = hdr.e_phnum;
	return load_bias + hdr.e_entry;
}

// vim:ts=4:sw=4

