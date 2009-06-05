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
	uint8_t * image;
	struct elf32_hdr	* hdr;
	struct elf32_phdr	* phdr_table;
	struct elf32_shdr	* shdr_table;
	const char			* snames; 
	struct elf32_shdr	** symtabs;
	const char			** strtabs;
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

extern  enum elf_file_type
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


// vim:ts=4:sw=4

