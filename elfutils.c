/* 
 * elfutils.c
 * by WN @ Jun. 04, 2009
 */

/* fixup type conflict between linux/types.h and include/types.h */
#define __KERNEL_STRICT_NAMES
typedef long long	__kernel_loff_t;
typedef __kernel_loff_t		loff_t;
#include <linux/elf.h>
#include "elfutils.h"
#include "debug.h"
#include "exception.h"

/* There's no 'struct elf32_shdr' in linux/elf.h,
 * the corresponding structure in that header is
 * a typedefed nameless struct  */
struct elf32_shdr {
	Elf32_Word	sh_name;		/* Section name (string tbl index) */
	Elf32_Word	sh_type;		/* Section type */
	Elf32_Word	sh_flags;		/* Section flags */
	Elf32_Addr	sh_addr;		/* Section virtual addr at execution */
	Elf32_Off	sh_offset;		/* Section file offset */
	Elf32_Word	sh_size;		/* Section size in bytes */
	Elf32_Word	sh_link;		/* Link to another section */
	Elf32_Word	sh_info;		/* Additional section information */
	Elf32_Word	sh_addralign;		/* Section alignment */
	Elf32_Word	sh_entsize;		/* Entry size if section holds table */
};

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

struct elf_handler *
elf_init(uint8_t * image, ptrdiff_t load_bias)
{
	struct elf_handler * newh = calloc(1, sizeof(*newh));
	/* Check for elf image */
	newh->image = image;

	struct elf32_hdr * hdr = (struct elf32_hdr *)image;

	TRACE(ELF, "checking elf image\n");

	if (memcmp(hdr->e_ident, ELFMAG, SELFMAG))
		THROW(EXCEPTION_FATAL, "ident doesn't match, not an elf image");
	if ((hdr->e_type != ET_EXEC) && (hdr->e_type != ET_DYN))
		THROW(EXCEPTION_FATAL, "image not an EXEc or DYN");
	if (hdr->e_machine != EM_386)
		THROW(EXCEPTION_FATAL, "not an i386 image");

	struct elf32_phdr * phdr_table =
		(struct elf32_phdr *)(image + hdr->e_phoff);
	struct elf32_shdr * shdr_table =
		(struct elf32_shdr *)(image + hdr->e_shoff);
	const char * snames = (const char *)
		(image + shdr_table[hdr->e_shstrndx].sh_offset);

	newh->hdr = hdr;
	newh->phdr_table = phdr_table;
	newh->shdr_table = shdr_table;
	newh->snames = snames;
	newh->load_bias = load_bias;

	/* iterator over sections */
	struct elf32_shdr ** symtabs = NULL;
	const char ** strtabs = NULL;
	int nr_symtabs = 0;
	for (int i = 0; i < hdr->e_shnum; i++) {
		if (shdr_table[i].sh_type == SHT_SYMTAB) {
			nr_symtabs ++;

			symtabs = realloc(symtabs, nr_symtabs * (sizeof(*symtabs)));
			symtabs[nr_symtabs - 1] = shdr_table + i;

			strtabs = realloc(strtabs, nr_symtabs * (sizeof(*strtabs)));
			int no_strtab = shdr_table[i].sh_link;
			strtabs[nr_symtabs - 1] = (const char *)(image + shdr_table[no_strtab].sh_offset);
			TRACE(ELF, "section [%d] is a symtab, corresponding strtab is section [%02d]\n",
					i, no_strtab);
		}

		if (shdr_table[i].sh_type == SHT_DYNAMIC) {
			TRACE(ELF, "section [%d] is dynamic section\n", i);
			newh->dynamic = shdr_table + i;
		}
	}

	newh->symtabs = symtabs;
	newh->strtabs = strtabs;
	newh->nr_symtabs = nr_symtabs;

	return newh;
}

void
elf_cleanup(struct elf_handler * h)
{
	if (h->symtabs != NULL)
		free(h->symtabs);
	if (h->strtabs != NULL)
		free(h->strtabs);
	free(h);
	return;
}

uintptr_t
elf_get_symbol_address(struct elf_handler * h,
		const char * sym)
{
	/* for each symtabs */
	for (int i = 0; i < h->nr_symtabs; i++) {
		struct elf32_shdr * sh = h->symtabs[i];
		int nr_entries = sh->sh_size / sh->sh_entsize;
		const char * strtab = h->strtabs[i];
		/* for each symbol */
		struct elf32_sym * syms = (struct elf32_sym*)
				(h->image + sh->sh_offset);
		for (int i = 0; i < nr_entries; i++) {
			const char * name = strtab + syms[i].st_name;
			uintptr_t value = syms[i].st_value;
			if (strcmp(sym, name) == 0) {
				/* found symbol */
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
	/* 1st: find the dynamic section */
	struct elf32_shdr * sh = h->dynamic;
	if (sh == NULL) {
		WARNING(ELF, "There's no '.dynamic' section\n");
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

	/* fill the dyn_info */
	dyn_info.strsh = &shdr_table[sh->sh_link];
	CTHROW(dyn_info.strsh->sh_type == SHT_STRTAB, "corrupted ELF file\n");
	TRACE(ELF, "dynamic strtab: [%d]\n", sh->sh_link);

	struct dyn_entry {
		uint32_t d_tag;
		uint32_t d_value;
	};

	struct dyn_entry * p = (struct dyn_entry *)(h->image + sh->sh_offset);
	for (int i = 0 ; (void*)(&p[i]) < ((void*)p) + sh->sh_size; i ++) {
		if (p[i].d_tag == DT_NULL)
			break;
		if (p[i].d_tag == DT_STRTAB) {
			CTHROW(p[i].d_value == dyn_info.strsh->sh_offset,
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


#define CHECKX(x, v)	CTHROW(dyn_info.x != v, "corrupted ELF file")
	CHECKX(symtab, NULL);
	CHECKX(reltab, NULL);
	CHECKX(strsh, NULL);
	CHECKX(strtab, NULL);
	CHECKX(relsz, 0);
	CHECKX(relent, 0);
#undef CHECKX
	dyn_info.nr_rel = dyn_info.relsz / dyn_info.relent;
	TRACE(ELF, "nr_recol=%d\n", dyn_info.nr_rel);

	/* for each recl entry */
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
			THROW(EXCEPTION_FATAL, "doesn't support image type 0x%x",
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


// vim:ts=4:sw=4

