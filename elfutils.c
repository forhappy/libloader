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
	ptrdiff_t load_biad;
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

	/* iterator over sections */
	for (int i = 0; i < hdr->e_shnum; i++) {
		TRACE(ELF, "section [%02d]: %s\n",
				i, snames + shdr_table[i].sh_name);
	}
	return NULL;
}

void
elf_cleanup(struct elf_handler * h)
{
	free(h);
	return;
}


// vim:ts=4:sw=4

