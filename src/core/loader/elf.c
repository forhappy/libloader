/* 
 * interp/elf.c
 * by WN @ Oct. 14, 2010
 * modified by HP.Fu @ Mar. 25, 2011
 */

#include <defs.h>
#include <debug.h>
#include <linux/elf.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/mman.h>
#include <loader/elf.h>
#include <loader/processor.h>

#include <syscall.h>


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

extern Elf32_Dyn _DYNAMIC[] ATTR_HIDDEN;
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
	for (Elf32_Dyn * dyn = &_DYNAMIC[0]; dyn->d_tag != DT_NULL; dyn ++) {
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
				/* FIXME! */
			case DT_RELA:
				breakpoint();
				break;
#if 0
			case DT_PLTGOT:
			case DT_PLTRELSZ:
			case DT_PLTREL:
			case DT_JMPREL:
#endif
		}
	}
	
	/* there's no relocation table */
	if (reltab == NULL)
		return;
	assert_noprint(relsz != 0);
	assert_noprint(relent != 0);
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
				/* B + A */
				real_val = old_val + load_bias;
				break;
			}
			case R_386_GLOB_DAT: {
				assert_noprint(symtab != NULL);
				int nsym = ELF32_R_SYM(info);
				struct elf32_sym * sym = &symtab[nsym];
				uint32_t sym_val = sym->st_value;
				/* S */
				real_val = sym_val + load_bias;
				break;
			}
			default: {
				/* printf may be unusable now */
				/* printf("relocate error: unknown relocate type 0x%x\n", type); */
				sys_exit(-1);
			}
		}
		*(uint32_t*)(address) = real_val;
	}
}


static int
open_elf_file(const char * fn, struct elfhdr * hdr)
{
	assert(fn != NULL);
	assert(hdr != NULL);

	int fd = sys_open(fn, O_RDONLY, 0);
	CASSERT(fd >= 0, LOADER,
			"open file %s failed: %d\n", fn, fd);

	int err = sys_read(fd, hdr, sizeof(*hdr));
	CASSERT(err == sizeof(*hdr),
			LOADER, "read elf header error: %d\n", err);

	CASSERT(memcmp(hdr->e_ident, ELFMAG, SELFMAG) == 0,
			LOADER, "magic check error\n");

	/* parse header */
	CASSERT(((hdr->e_type == ET_DYN) || (hdr->e_type == ET_EXEC)),
			LOADER, "doesn't support type %d\n",
			hdr->e_type);
	CASSERT(hdr->e_machine == EM_386,
			LOADER, "doesn't support machine: %d\n",
			hdr->e_machine);
	return fd;
}

static void
load_phdrs(int fd, struct elf_phdr * dest, struct elfhdr * h)
{
	int err;
	err = sys_lseek(fd, h->e_phoff, SEEK_SET);
	assert(err >= 0);
	err = sys_read(fd, dest, sizeof(*dest) * h->e_phnum);
	assert(err == (int)(sizeof(*dest) * h->e_phnum));
}

static size_t
get_total_map_sz(struct elf_phdr * phdrs, int nr_phdrs, int * first)
{
	assert(phdrs != NULL);
	assert(first != NULL);

	uintptr_t start = (uintptr_t)(-1);
	uintptr_t end = 0;
	for (struct elf_phdr * p = phdrs; p < &phdrs[nr_phdrs]; p ++) {
		if (p->p_type != PT_LOAD)
			continue;

		if (start > p->p_vaddr) {
			start = p->p_vaddr;
			*first = p - phdrs;
		}

		uintptr_t p_end = p->p_vaddr + p->p_memsz;
		if (end < p_end)
			end = p_end;
		//assert(p->p_align == 0x1000);
		assert((
					(uintptr_t)(p->p_vaddr) -
					(uintptr_t)(p->p_offset)
					) % p->p_align == 0);
	}

	start = ALIGN_DOWN_PTR(start, 0x1000);
	end = ALIGN_UP_PTR(end, 0x1000);

	assert(end - start > 0);

	return end - start;
}

#define get_map_prot(___f) ({			\
		uint32_t ___map_prot = 0;		\
		if ((___f) & PF_R)				\
			___map_prot |= PROT_READ;	\
		if ((___f) & PF_W)				\
			___map_prot |= PROT_WRITE;	\
		if ((___f) & PF_X)				\
			___map_prot |= PROT_EXEC;	\
		___map_prot;					\
		})

static void *
map_first(int fd, struct elf_phdr * first_phdr,
		size_t total_map_sz, void ** p_bias, bool_t fixed)
{
	void * start_addr = (void*)(first_phdr->p_vaddr);
	void * end_addr = start_addr + total_map_sz;

	start_addr = ALIGN_DOWN_PTR(start_addr, 0x1000);
	end_addr = ALIGN_UP_PTR(end_addr, 0x1000);
	total_map_sz = end_addr - start_addr;

	uint32_t map_type = MAP_PRIVATE;
	if (fixed)
		map_type |= MAP_FIXED;

	/* map the first segment with total_map_sz, and 
	 * unmap unneed pages. This is to make sure we have enough
	 * continuous memory space to hold the whole mem image. */
	assert(first_phdr->p_offset == 0);

	uint32_t prot = get_map_prot(first_phdr->p_flags);
	void * map_addr = sys_uniform_mmap(fixed ? start_addr : NULL,
			total_map_sz, prot,
			map_type, fd, first_phdr->p_offset);

	void * load_bias = fixed ? 0 :
		map_addr - first_phdr->p_vaddr;

	TRACE(LOADER, "load_bias=%p\n", load_bias);

	if (!fixed) {
		start_addr = load_bias + (uintptr_t)(start_addr);
		end_addr = load_bias + (uintptr_t)(end_addr);
	}

	/* unmap the padding pages */
	void * unmap_start = start_addr
		+ first_phdr->p_memsz;
	void * unmap_end = end_addr;

	unmap_start = ALIGN_UP_PTR(unmap_start, 4096);
	unmap_end = ALIGN_UP_PTR(unmap_end, 4096);
	TRACE(LOADER, "unmap from %p to %p\n",
			unmap_start, unmap_end);
	if (unmap_end > unmap_start)
		sys_munmap(unmap_start, unmap_end - unmap_start);

	/* pad first physical region */
	if ((first_phdr->p_filesz < first_phdr->p_memsz) &&
			(first_phdr->p_flags & PF_W)) {
		memset(start_addr + first_phdr->p_filesz,
				'\0',
				first_phdr->p_memsz - first_phdr->p_filesz);
	}

	if (p_bias != NULL)
		*p_bias = load_bias;

	return start_addr;
}

static void
map_region(int fd, struct elf_phdr * phdr, void * load_bias)
{
	void * start = load_bias + phdr->p_vaddr;
	void * f_end = start + phdr->p_filesz;
	void * m_end = start + phdr->p_memsz;

	void * start_al = ALIGN_DOWN_PTR(start, 0x1000);
	void * f_end_al = ALIGN_UP_PTR(f_end, 0x1000);
	void * m_end_al = ALIGN_UP_PTR(m_end, 0x1000);

	uint32_t port = get_map_prot(phdr->p_flags);
	void * addr = sys_uniform_mmap(start_al, f_end_al - start_al, port,
			MAP_PRIVATE | MAP_FIXED, fd, phdr->p_offset);
	TRACE(LOADER, "addr=%p, start_al=%p\n", addr, start_al);
	assert(addr == start_al);
	TRACE(LOADER, "map file from %p to %p\n", addr, f_end_al);

	/* map additional pages */
	if (m_end_al > f_end_al) {
		addr = sys_uniform_mmap(f_end_al, m_end_al - f_end_al, port,
				MAP_PRIVATE | MAP_ANONYMOUS | MAP_DENYWRITE | MAP_FIXED,
				0, 0);
		assert(addr == f_end_al);
	}

	/* zero padding area */
	memset(f_end, '\0', m_end_al - f_end);
}


void *
load_elf(const char * fn, void ** p_load_bias,
		struct elf_phdr ** pp_phdrs, int * p_nr_phdrs,
		bool_t * p_need_interp)
{
	assert(fn != NULL);

	TRACE(LOADER, "loading elf file %s\n", fn);
	struct elfhdr hdr;
	int fd = open_elf_file(fn, &hdr);

	int nr_phdrs = hdr.e_phnum;

	struct elf_phdr * phdrs = alloca(sizeof(struct elf_phdr) * nr_phdrs);
	assert(phdrs != NULL);
	load_phdrs(fd, phdrs, &hdr);

	int first = 0;
	size_t total_map_sz = get_total_map_sz(phdrs, nr_phdrs, &first);
	TRACE(LOADER, "total_map_sz=0x%lx\n", (long int)total_map_sz);

	/* map first region */
	void * load_bias = NULL;
	void * map_addr = map_first(fd, &phdrs[first],
			total_map_sz, &load_bias, hdr.e_type == ET_EXEC);

	/* map other regions */
	if (p_need_interp != NULL)
		*p_need_interp = FALSE;
	for (int i = 0; i < nr_phdrs; i++) {
		if (phdrs[i].p_type == PT_INTERP) {
			if (p_need_interp != NULL)
				*p_need_interp = TRUE;
		}

		if (i == first)
			continue;
		if (phdrs[i].p_type != PT_LOAD)
			continue;

		map_region(fd, &phdrs[i], load_bias);
	}

	sys_close(fd);
	if (p_load_bias != NULL)
		*p_load_bias = load_bias;
	if (pp_phdrs != NULL)
		*pp_phdrs = map_addr + hdr.e_phoff;
	if (p_nr_phdrs != NULL)
		*p_nr_phdrs = hdr.e_phnum;

	return load_bias + hdr.e_entry;
}
// vim:ts=4:sw=4

