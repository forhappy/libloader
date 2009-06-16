/* 
 * elfutils.h
 * by WN @ Jun. 04, 2009
 */

#ifndef ELFUTIILS_H
#define ELFUTIILS_H

#include <sys/cdefs.h>
#include <stdint.h>
#include "defs.h"

struct elf_handler;

/* load the desired file and check
 * whether it is a valid and sutable ELF file */
extern struct elf_handler *
elf_init(uint8_t * image, ptrdiff_t load_bias);

extern void
elf_cleanup(struct elf_handler * h);

/* return the address of a symbol,
 * 0 means not found */
extern uintptr_t
elf_get_symbol_address(struct elf_handler * h,
		const char * sym);

enum elf_file_type {
	ELF_DYN,
	ELF_EXEC,
	ELF_NOSUPPORT,
};

extern  enum elf_file_type
elf_get_image_type(struct elf_handler * h);

struct elf32_phdr *
elf_get_phdr_table(struct elf_handler * h, int * nr_phdrs);

#endif

