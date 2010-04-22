/* 
 * snitchaser elf.h
 * by WN @ Apr. 21, 2010
 */

#ifndef __SNITCHASER_ELF_H
#define __SNITCHASER_ELF_H

#include <common/defs.h>
#include <xasm/elf.h>

uintptr_t
elf_find_symbol(const char * elf_fn, ptrdiff_t load_bias, const char * name);


#endif

// vim:ts=4:sw=4

