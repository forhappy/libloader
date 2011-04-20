#include "xconfig.h"
#include <linux/kernel.h>
#include <linux/personality.h>

#include "defs.h"
#include "debug.h"
#include "malloc.h"
#include "vsprintf.h"
#include "exception.h"
#include "elf.h"
#include "startup_stack.h"
#include "env_conf.h"
#include "syscall.h"
#include "processor.h"

#define R_386_NONE	   0		/* No reloc */
#define R_386_32	   1		/* Direct 32 bit  */
#define R_386_PC32	   2		/* PC relative 32 bit */
#define R_386_GOT32	   3		/* 32 bit GOT entry */
#define R_386_PLT32	   4		/* 32 bit PLT address */
#define R_386_COPY	   5		/* Copy symbol at runtime */
#define R_386_GLOB_DAT	   6		/* Create GOT entry */
#define R_386_JMP_SLOT	   7		/* Create PLT entry */
#define R_386_RELATIVE	   8		/* Adjust by program base */
#define R_386_GOTOFF	   9		/* 32 bit offset to GOT */
#define R_386_GOTPC	   10		/* 32 bit PC relative offset to GOT */
#define R_386_32PLT	   11
#define R_386_TLS_TPOFF	   14		/* Offset in static TLS block */
#define R_386_TLS_IE	   15		/* Address of GOT entry for static TLS
					   block offset */
#define R_386_TLS_GOTIE	   16		/* GOT entry for static TLS block
					   offset */
#define R_386_TLS_LE	   17		/* Offset relative to static TLS
					   block */
#define R_386_TLS_GD	   18		/* Direct 32 bit for GNU version of
					   general dynamic thread local data */
#define R_386_TLS_LDM	   19		/* Direct 32 bit for GNU version of
					   local dynamic thread local data
					   in LE code */
#define R_386_16	   20
#define R_386_PC16	   21
#define R_386_8		   22
#define R_386_PC8	   23
#define R_386_TLS_GD_32	   24		/* Direct 32 bit for general dynamic
					   thread local data */
#define R_386_TLS_GD_PUSH  25		/* Tag for pushl in GD TLS code */
#define R_386_TLS_GD_CALL  26		/* Relocation for call to
					   __tls_get_addr() */
#define R_386_TLS_GD_POP   27		/* Tag for popl in GD TLS code */
#define R_386_TLS_LDM_32   28		/* Direct 32 bit for local dynamic
					   thread local data in LE code */
#define R_386_TLS_LDM_PUSH 29		/* Tag for pushl in LDM TLS code */
#define R_386_TLS_LDM_CALL 30		/* Relocation for call to
					   __tls_get_addr() in LDM code */
#define R_386_TLS_LDM_POP  31		/* Tag for popl in LDM TLS code */
#define R_386_TLS_LDO_32   32		/* Offset relative to TLS block */
#define R_386_TLS_IE_32	   33		/* GOT entry for negated static TLS
					   block offset */
#define R_386_TLS_LE_32	   34		/* Negated offset relative to static
					   TLS block */
#define R_386_TLS_DTPMOD32 35		/* ID of module containing symbol */
#define R_386_TLS_DTPOFF32 36		/* Offset in TLS block */
#define R_386_TLS_TPOFF32  37		/* Negated offset in static TLS block */
/* 38? */
#define R_386_TLS_GOTDESC  39		/* GOT offset for TLS descriptor.  */
#define R_386_TLS_DESC_CALL 40		/* Marker of call through TLS
					   descriptor for
					   relaxation.  */
#define R_386_TLS_DESC     41		/* TLS descriptor containing
					   pointer to code and to
					   argument, returning the TLS
					   offset for the symbol.  */
#define R_386_IRELATIVE	   42		/* Adjust indirectly by program base */
/* Keep this the last entry.  */
#define R_386_NUM	   43
#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#define O_CREAT		   0100	/* not fcntl */
#define O_EXCL		   0200	/* not fcntl */
#define O_NOCTTY	   0400	/* not fcntl */
#define O_TRUNC		  01000	/* not fcntl */
#define O_APPEND	  02000
#define O_NONBLOCK	  04000
#define O_NDELAY	O_NONBLOCK
#define O_SYNC	       04010000
#define O_FSYNC		 O_SYNC
#define O_ASYNC		 020000

#ifdef __ASSEMBLY__
#define _AC(X,Y)	X
#define _AT(T,X)	X
#else
#define __AC(X,Y)	(X##Y)
#define _AC(X,Y)	__AC(X,Y)
#define _AT(T,X)	((T)(X))
#endif

#define PAGE_SHIFT	12
#define PAGE_SIZE	(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define ELF_EXEC_PAGESIZE	4096

#if ELF_EXEC_PAGESIZE > PAGE_SIZE
#define ELF_MIN_ALIGN	ELF_EXEC_PAGESIZE
#else
#define ELF_MIN_ALIGN	PAGE_SIZE
#endif

#ifndef ELF_CORE_EFLAGS
#define ELF_CORE_EFLAGS	0
#endif
#define ELF_PAGESTART(_v) ((_v) & ~(unsigned long)(ELF_MIN_ALIGN-1))
#define ELF_PAGEOFFSET(_v) ((_v) & (ELF_MIN_ALIGN-1))
#define ELF_PAGEALIGN(_v) (((_v) + ELF_MIN_ALIGN - 1) & ~(ELF_MIN_ALIGN - 1))

#define ET_NONE		0		/* No file type */
#define ET_REL		1		/* Relocatable file */
#define ET_EXEC		2		/* Executable file */
#define ET_DYN		3		/* Shared object file */
#define ET_CORE		4		/* Core file */
#define	ET_NUM		5		/* Number of defined types */

enum elf_file_type {
	ELF_DYN,
	ELF_EXEC,
	ELF_NOSUPPORT,
};
#define PROT_READ	0x1		/* page can be read */
#define PROT_WRITE	0x2		/* page can be written */
#define PROT_EXEC	0x4		/* page can be executed */
#define PROT_SEM	0x8		/* page may be used for atomic ops */
#define PROT_NONE	0x0		/* page can not be accessed */
#define PROT_GROWSDOWN	0x01000000	/* mprotect flag: extend change to start of growsdown vma */
#define PROT_GROWSUP	0x02000000	/* mprotect flag: extend change to end of growsup vma */

#define MAP_EXECUTABLE	0x1000		/* mark it as an executable */
#define MAP_SHARED	0x01		/* Share changes */
#define MAP_PRIVATE	0x02		/* Changes are private */
#define MAP_TYPE	0x0f		/* Mask for type of mapping */
#define MAP_FIXED	0x10		/* Interpret addr exactly */
#define MAP_ANONYMOUS	0x20		/* don't use a file */
void xfree(void * ptr, size_t size);
void * xmalloc(size_t size);
void * xrealloc(void * old_ptr, size_t old_size, size_t new_size);
void * xcalloc(size_t nmenb, size_t size);
struct elf_handler {
    void * image;
    struct elf32_hdr	* hdr;
    struct elf32_phdr	* phdr_table;
    struct elf32_shdr	* shdr_table;
    const char			* snames;
    struct elf32_shdr	** symtabs;
    const char			** strtabs;
    struct elf32_shdr   * dynamic;
    int nr_symtabs;
    ptrdiff_t load_bias;
};

struct elf_file {
    void *mm;
    struct elf_handler *handler;
    char *fn;
};

static struct elf_file wrapper = {NULL, NULL, "./libwrapper.so"};

uint32_t * vdso_entry = NULL;
 
uint32_t * vdso_ehdr = NULL;

uint32_t old_vdso_entry = 0;

/* below 2 vars are for sigaction and rt_sigaction */
uint32_t wrapped_sigreturn = 0;
uint32_t wrapped_rt_sigreturn = 0;
uint32_t wrapped_sighandler = 0;
void *
xcalloc(size_t nmenb, size_t size)
{
	size_t sz;
	assert(nmenb > 0);
	assert(size > 0);
	TRACE(MEM, "allocating 0x%lx bytes of memory\n", (long int)nmenb * size);
	sz = ALIGN_UP(nmenb * size, 4);
	int prot = PROT_READ | PROT_WRITE;
	void * ptr = sys_uniform_mmap(NULL, sz, prot,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert(is_valid_ptr(ptr));
	TRACE(MEM, "allocated 0x%lx bytes of memory %p\n", (long int)nmenb * size, ptr);
	return ptr;
}
void *
xmalloc(size_t size)
{
	size_t sz;
	assert(size > 0);
	TRACE(MEM, "allocating 0x%lx bytes of memory\n", (long int)size);
	sz = ALIGN_UP(size , 4);
	int prot = PROT_READ | PROT_WRITE;
	void *ptr = sys_uniform_mmap(NULL, sz, prot,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert(is_valid_ptr(ptr));
	TRACE(MEM, "allocated 0x%lx bytes of memory %p\n", (long int)size, ptr);
	return ptr;
}

/*
The  realloc() function changes the size of the memory block pointed to by ptr
to size bytes.  The contents will be unchanged in the range from the start of
the region up to the minimum of the old and new sizes.  If the new size is
larger than the old size, the added memory will not be initial‐ized.  If ptr is
NULL, then the call is equivalent to malloc(size), for all values of size; if
size is equal to zero, and ptr is not NULL, then the call is equivalent to
free(ptr).  Unless ptr is NULL, it must have been returned by an earlier call to
malloc(), calloc()  or  realloc().   If  the area pointed to was moved, a
free(ptr) is done.
*/
void *
xrealloc(void *old_ptr, size_t old_size, size_t size)
{
	void *ptr = NULL;

	if (ptr == NULL) {
		return xmalloc(size);
	}
	if (size == 0 && ptr != NULL) {
		xfree(old_ptr, old_size);
	}
	if (size > old_size) {
		TRACE(MEM, "reallocating 0x%lx bytes of memory at %p\n", (long int)size, ptr);
		size = ALIGN_UP(size , 4);
		void *ptr = sys_mremap(old_ptr, old_size, size,
				MAP_PRIVATE | MAP_ANONYMOUS, old_ptr);
		assert(is_valid_ptr(ptr));
		TRACE(MEM, "allocated 0x%lx bytes of memory %p\n", (long int)size, ptr);
		return ptr;
	}
	// FIXME size < old_size not supported 
	return old_ptr;
}

void
xfree(void * ptr, size_t sz)
{
	assert(is_valid_ptr(ptr));
	TRACE(MEM, "deallocating 0x%lx bytes of pages: %p\n", (long int)sz, ptr);
	sz = ALIGN_UP(sz,4);
	int err = sys_munmap(ptr, sz);
	assert(err == 0);
}

void *xmemset(void *s, int c, size_t count)
{
	char *xs = s;

	while (count--)
		*xs++ = c;
	return s;
}

struct elf_handler *
elf_init(uint8_t * image, ptrdiff_t bias)
//将image指向的内存区域识别为elf格式,并返回elf_handler结果。
{
	struct elf_handler * handler = xcalloc(1, sizeof(*handler));
	handler->image = image;
	struct elf32_hdr * hdr = (struct elf32_hdr *)image;
	DEBUG(ELF, "checking elf image!\n");
    if (hdr->e_ident[EI_MAG0] != 0x7f
			|| hdr->e_ident[EI_MAG1] != 'E'
            || hdr->e_ident[EI_MAG2] != 'L'
            || hdr->e_ident[EI_MAG3] != 'F')
		THROW(EXP_ELF, "ident doesn't match, not an elf image");
	if ((hdr->e_type != ET_EXEC) && (hdr->e_type != ET_DYN))
		THROW(EXP_ELF, "image not an EXEc or DYN");
	if (hdr->e_machine != EM_386)
		THROW(EXP_ELF, "not an i386 image");

	struct elf32_phdr * phdr_table =
		(struct elf32_phdr *)(image + hdr->e_phoff);
	struct elf32_shdr * shdr_table =
		(struct elf32_shdr *)(image + hdr->e_shoff);
	const char * snames = (const char *)
		(image + shdr_table[hdr->e_shstrndx].sh_offset);

	handler->hdr = hdr;
	handler->phdr_table = phdr_table;
	handler->shdr_table = shdr_table;
	handler->snames = snames;
	handler->load_bias = bias;

	// iterator over sections 
	struct elf32_shdr ** symtabs = NULL;
	const char ** strtabs = NULL;
	int nr_symtabs = 0;
	for (int i = 0; i < hdr->e_shnum; i++) {
		//解析节区表；
		if (shdr_table[i].sh_type == SHT_SYMTAB) {
			nr_symtabs ++;

			symtabs = xrealloc(symtabs, (nr_symtabs - 1) * (sizeof(*symtabs)), nr_symtabs * (sizeof(*symtabs)));
			symtabs[nr_symtabs - 1] = shdr_table + i;

			strtabs = xrealloc(strtabs, (nr_symtabs - 1) * (sizeof(*strtabs)), nr_symtabs * (sizeof(*strtabs)));
			int no_strtab = shdr_table[i].sh_link;
			strtabs[nr_symtabs - 1] = (const char *)(image + shdr_table[no_strtab].sh_offset);
			TRACE(ELF, "section [%d] is a symtab, corresponding strtab is section [%02d]\n",
					i, no_strtab);
		}

		if (shdr_table[i].sh_type == SHT_DYNAMIC) {
			TRACE(ELF, "section [%d] is dynamic section\n", i);
			handler->dynamic = shdr_table + i;
		}
	}

	handler->symtabs = symtabs;
	handler->strtabs = strtabs;
	handler->nr_symtabs = nr_symtabs;

	return handler;
}

void
elf_cleanup(struct elf_handler * h)
{
	if (h->symtabs != NULL)
		xfree(h->symtabs, sizeof(h->symtabs));
	if (h->strtabs != NULL)
		xfree(h->strtabs, sizeof(h->strtabs));
    xfree(h, sizeof(*h));
	return;
}

uintptr_t
elf_get_symbol_address(struct elf_handler * h,
		const char * sym)
{
	// for each symtabs 
	for (int i = 0; i < h->nr_symtabs; i++) {
		struct elf32_shdr * sh = h->symtabs[i];
		int nr_entries = sh->sh_size / sh->sh_entsize;
		const char * strtab = h->strtabs[i];
		// for each symbol 
		struct elf32_sym * syms = (struct elf32_sym*)
				(h->image + sh->sh_offset);
		for (int i = 0; i < nr_entries; i++) {
			const char * name = strtab + syms[i].st_name;
			uintptr_t value = syms[i].st_value;
			if (strcmp(sym, name) == 0) {
				// found symbol
				if (value == 0)
					return 0;
				return value + h->load_bias;
			}
		}
	}
	return 0;
}

void
elf_reloc_symbols(struct elf_handler * h,
		void (*fn)(uintptr_t addr, const char * sym,
			int type, int sym_val))
{
	struct elf32_shdr * shdr_table = h->shdr_table;
	// 1st: find the dynamic section 
	struct elf32_shdr * sh = h->dynamic;
	if (sh == NULL) {
		WARNING(ELF, "no '.dynamic' section\n");
		return;
	}

	struct {
		struct elf32_sym * symtab;
		struct elf32_rel * reltab;
		struct elf32_shdr * strsh;
		const char * strtab;
		int relsz;
		int relent;
		int nr_rel;
	} dyn_info;

	memset(&dyn_info, '\0', sizeof(dyn_info));

	// fill the dyn_info 
	dyn_info.strsh = &shdr_table[sh->sh_link];
	CTHROW(dyn_info.strsh->sh_type == SHT_STRTAB, EXP_ELF, "corrupted ELF file\n");
	TRACE(EXP_ELF, "dynamic strtab: [%d]\n", sh->sh_link);

	struct dyn_entry {
		uint32_t d_tag;
		uint32_t d_value;
	};

	struct dyn_entry * p = (struct dyn_entry *)(h->image + sh->sh_offset);
	for (int i = 0 ; (void*)(&p[i]) < ((void*)p) + sh->sh_size; i ++) {
		if (p[i].d_tag == DT_NULL)
			break;
		if (p[i].d_tag == DT_STRTAB) {
			CTHROW(p[i].d_value == dyn_info.strsh->sh_offset, EXP_ELF,
					"corrupted ELF file\n");
			dyn_info.strtab = h->image +  p[i].d_value;
		}
		if (p[i].d_tag == DT_SYMTAB)
			dyn_info.symtab = h->image + p[i].d_value;
		if (p[i].d_tag == DT_REL)
			dyn_info.reltab = h->image + p[i].d_value;
		if (p[i].d_tag == DT_RELSZ)
			dyn_info.relsz = p[i].d_value;
		if (p[i].d_tag == DT_RELENT)
			dyn_info.relent = p[i].d_value;
	}

	if (dyn_info.reltab == NULL) {
		WARNING(ELF, "there's no reltable in this image\n");
		return;
	}


#define CHECKX(x, v)	CTHROW(dyn_info.x != v, EXP_ELF, "corrupted ELF file")
	CHECKX(symtab, NULL);
	CHECKX(reltab, NULL);
	CHECKX(strsh, NULL);
	CHECKX(strtab, NULL);
	CHECKX(relsz, 0);
	CHECKX(relent, 0);
#undef CHECKX
	dyn_info.nr_rel = dyn_info.relsz / dyn_info.relent;
	TRACE(ELF, "nr_recol=%d\n", dyn_info.nr_rel);

	// for each recl entry 
	for (int i = 0; i < dyn_info.nr_rel; i++) {
		uint32_t offset;
		uint32_t info;
		offset = dyn_info.reltab[i].r_offset;
		info = dyn_info.reltab[i].r_info;

		int type = ELF32_R_TYPE(info);
		int nsym = ELF32_R_SYM(info);

		struct elf32_sym * sym = &dyn_info.symtab[nsym];
		const char * name = sym->st_name + dyn_info.strtab;

		TRACE(ELF, "find relsym %s, offset=0x%x, info=0x%x, value=0x%x\n",
				name, offset, info, sym->st_value);
		if (fn != NULL)
			fn(offset + h->load_bias, name, type, sym->st_value);
	}
}


enum elf_file_type
elf_get_image_type(struct elf_handler * h)
{
	switch (h->hdr->e_type) {
		case ET_EXEC:
			return ELF_EXEC;
		case ET_DYN:
			return ELF_DYN;
		default:
			THROW(EXP_ELF, "doesn't support image type 0x%x",
					h->hdr->e_type);
			return ELF_NOSUPPORT;
	}
}

struct elf32_phdr *
elf_get_phdr_table(struct elf_handler * h, int * nr_phdrs)
{
	if (nr_phdrs != NULL)
		*nr_phdrs = h->hdr->e_phnum;
	return h->phdr_table;
}

int 
load_file(const char *fn)
{

	assert(fn != NULL);
	int fd = sys_open(fn, O_RDONLY, 0);
	CASSERT(fd >= 0, LOADER,
			"open file %s failed: %d\n", fn, fd);

	return fd;
}
static void
map_wrapper(void)
{
	int err;
	int nr;
	struct elf32_phdr * phdr = elf_get_phdr_table(wrapper.handler, &nr);
	CTHROW((phdr != NULL) && (nr != 0),EXP_ELF,"load phdr of injector file %s failed", wrapper.fn);

	/* open the file */
	/* +1 for the last '\0' */
	/*
	uint32_t fn_pos = ptrace_push(injector.fn, strlen(injector.fn) + 1, TRUE);
	int fd = ptrace_syscall(open, 3, fn_pos, O_RDONLY, 0);
	CTHROW(fd >= 0, "open injector from client code failed, return %d", fd);
	SYS_TRACE("open injector for child, fd=%d\n", fd);
	*/
	int fd = sys_open("./libwrapper.so", O_RDWR, 0); 
	/* for each program header */
	for (int i = 0; i < nr; i++, phdr++) {
		TRACE(LOADER, "phdr %d, type=0x%x, flag=0x%x\n",
				i, phdr->p_type, phdr->p_flags);
		if (phdr->p_type != PT_LOAD)
			continue;

		int elf_prot = 0, elf_flags = 0;
		if (phdr->p_flags & PF_R)
			elf_prot |= PROT_READ;
		if (phdr->p_flags & PF_X)
			elf_prot |= PROT_EXEC;
		elf_prot |= PROT_WRITE;
		elf_flags = MAP_PRIVATE | MAP_EXECUTABLE;

		unsigned long size = phdr->p_filesz + ELF_PAGEOFFSET(phdr->p_vaddr);
		unsigned long off = phdr->p_offset - ELF_PAGEOFFSET(phdr->p_vaddr);
		int32_t map_addr = /*opts->inj_bias*/0x3000 + phdr->p_vaddr - ELF_PAGEOFFSET(phdr->p_vaddr);

		TRACE(LOADER,"map to 0x%x, size=0x%x, memsz=0x%x, off=0x%x\n", map_addr, size, phdr->p_memsz, off);

		uint32_t ret_addr = sys_mmap2((void *)map_addr, size, elf_prot,
				elf_flags | MAP_FIXED, fd, off >> PAGE_SHIFT);
		CTHROW(map_addr == ret_addr, EXP_ELF, "map injector failed, return 0x%x", ret_addr);

		if (phdr->p_memsz > size) {
			/* we need to map additional 0 pages */
			uint32_t add_sz = ELF_PAGEALIGN(phdr->p_memsz - ELF_PAGEALIGN(size));
			uint32_t start = ELF_PAGEALIGN(map_addr + size);

			TRACE(LOADER, "meed additional zeroed page: memsz=0x%x, mapped size=0x%x, additional size=0x%x\n",
					phdr->p_memsz, size, add_sz);

			uint32_t ret_addr = sys_mmap2((void *)start, add_sz, elf_prot,
					elf_flags | MAP_FIXED | MAP_ANONYMOUS, 0, 0);
			CTHROW(ret_addr == start, EXP_ELF,
					"map injector additional page failed, return %x", ret_addr);

			/* zero those pages */
			int zsz = phdr->p_memsz - size;
			void * zeros = xcalloc(1, zsz);
			memcpy((void *)map_addr + size, zeros, zsz);
			xfree(zeros, zsz);
		}
	}

	/* close the file */
	err = sys_close(fd);
	if (err != 0)
		WARNING(SYSTEM, "close injector in child code failed: %d\n", err);
}

static void
reloc_wrapper(uintptr_t addr, const char * sym, int type, int sym_val)
{

	TRACE(LOADER, "relocate symbol '%s' at address 0x%x\n", sym, addr);

	if ((type != R_386_PC32) &&
			(type != R_386_32) &&
			(type != R_386_RELATIVE) &&
			(type != R_386_GLOB_DAT)) {
		WARNING(LOADER, "doesn't support reloc type 0x%x", type);
		return;
	}
	
	uint32_t real_val;

	if (sym[0] == '\0') {
		if (type != R_386_RELATIVE) {
			WARNING(LOADER, "don't know how to relocate this type: 0x%x\n",
					type);
			return;
		}
		real_val = 0;
	} else {
		if (strncmp(sym, "__vsyscall", strlen("__vsyscall")) == 0) {
			real_val = old_vdso_entry;
		} else if (type != R_386_GLOB_DAT) {
			WARNING(LOADER, "don't know how to relocate this symbol: '%s'\n",
					sym);
			return;
		}
	}


	switch (type) {
		case R_386_32: {
			break;
		}
		case R_386_PC32: {
			int32_t old_val;
			memcpy(&old_val, (void *)addr, sizeof(old_val));
			real_val = real_val + old_val - addr;
			break;
		}
		case R_386_RELATIVE: {
			int32_t old_val;
			memcpy(&old_val, (void *)addr, sizeof(old_val));
			real_val = old_val + 0x3000;
			break;
		}
		case R_386_GLOB_DAT: {
			real_val = sym_val + 0x3000;
			break;
		}
	}

	memcpy((void *)addr, &real_val,  sizeof(real_val));
	TRACE(LOADER, "\tset to 0x%x\n", real_val);
	return;
}

#define K_NSIG	(64)
#define _NSIG_WORDS	(2)
#if 0
typedef struct {
	unsigned long sig[_NSIG_WORDS];
} k_sigset_t;

struct state_vector {
	int dummy;
	uint32_t brk;
	pid_t pid;
	uint32_t clear_child_tid;
	uint32_t robust_list;
	struct user_desc thread_area[GDT_ENTRY_TLS_ENTRIES];
	/* signal 0 is no use */
	struct k_sigaction sigactions[K_NSIG+1];
	k_sigset_t sigmask;
	/* add user regs into state_vector, this is for only
	 * checkpoints use */
	struct user_regs_struct regs;
	struct i387_fxsave_struct fpustate;
	int end;
} state_vector;
#endif
#define LOGGER_DIRECTORY "/tmp/snitchaser/"
int
xlogger_init(pid_t pid)
{

	int fd;
	char filename[128];
	struct timeval tv;
	int err = sys_gettimeofday(&tv, NULL);
	CASSERT(err == 0, LOGGER, "sys_gettimeofday failed\n");
	size_t fn_sz = snprintf(filename, 128 - 1, \
			LOGGER_DIRECTORY"%d-%d-%010u-%010u.log", sys_getpid(), sys_gettid(), \
			(uint32_t)(tv.tv_sec), (uint32_t)(tv.tv_usec));
	CASSERT(fn_sz < 128 - 1, LOGGER, "fn_sz <> 128\n");
	fd = sys_open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0){
		WARNING(LOADER, "open ckpt file %s failed\n", filename);
		return 1;
	}
	return fd;
}

int
xmemckpt_init(pid_t pid)
{

	int fd;
	char filename[128];
	struct timeval tv;
	int err = sys_gettimeofday(&tv, NULL);
	CASSERT(err == 0, LOGGER, "sys_gettimeofday failed\n");
	size_t fn_sz = snprintf(filename, 128 - 1, \
			LOGGER_DIRECTORY"%d-%d-%010u-%010u.ckpt", sys_getpid(), sys_gettid(), \
			(uint32_t)(tv.tv_sec), (uint32_t)(tv.tv_usec));
	CASSERT(fn_sz < 128 - 1, LOGGER, "fn_sz > 128 bytes\n");
	fd = sys_open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0){
		WARNING(LOADER, "open ckpt file %s failed\n", filename);
		return 1;
	}
	return fd;
}
int SCOPE ckpt_fd = 0;
int SCOPE logger_fd = 0;
uint32_t SCOPE logger_sz = 0;

__attribute__((used, unused, visibility("hidden"))) void
snitchaser_main(struct snitchaser_startup_stack * stack)
{

	logger_fd = xlogger_init(sys_getpid());
	ckpt_fd = xmemckpt_init(sys_getpid());
	flush_mem_to_ckpt(ckpt_fd);
	return;
}
