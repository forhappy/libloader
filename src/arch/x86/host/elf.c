/* 
 * x86 elf.c
 * by WN @ Apr. 21, 2010
 */

#include <common/debug.h>
#include <host/exception.h>
#include <host/elf.h>
#include <host/mm.h>

#include <assert.h>
#include <string.h>
#include <bfd.h>

uintptr_t
elf_find_symbol(const char * elf_fn, ptrdiff_t load_bias, const char * name)
{
	assert(elf_fn != NULL);
	bool_t found = FALSE;
	uintptr_t ret = 0;

	bfd_init();
	bfd * bfd = bfd_openr(elf_fn, NULL);
	if (bfd == NULL)
		THROW_FATAL(EXP_ELF, "bfd open file %s failed", elf_fn);

	define_exp(exp);
	TRY(exp) {

		if (!bfd_check_format(bfd, bfd_object))
			THROW_FATAL(EXP_ELF, "not object file");
		if (!(bfd_get_file_flags(bfd) & HAS_SYMS))
			THROW_FATAL(EXP_ELF, "object file doesn't contain symbols");

		void * minisyms;
		unsigned int size;
		int symcount = bfd_read_minisymbols(bfd, 0, &minisyms, &size);

		if (minisyms == NULL)
			THROW_FATAL(EXP_ELF, "minisyms is null");

		asymbol * store = bfd_make_empty_symbol(bfd);
		if (store == NULL)
			THROW_FATAL(EXP_ELF, "bfd_make_empty_symbol failed");

		bfd_byte * from, * fromend;
		from = (bfd_byte *)minisyms;
		fromend = from + symcount * size;
		
		for (; from < fromend; from += size) {
			asymbol * sym;
			sym = bfd_minisymbol_to_symbol(bfd, 0,
					(const void *)from, store);
			symbol_info syminfo;
			bfd_get_symbol_info(bfd, sym, &syminfo);
			if (strcmp(name, sym->name) == 0) {
				/* found */
				found = TRUE;
				ret = (uintptr_t)syminfo.value + load_bias;
				break;
			}
		}

	} FINALLY {
		bfd_close(bfd);
	} CATCH(exp) {
		RETHROW(exp);
	}

	if (found)
		return ret;
	else
		THROW(EXP_ELF_SYMBOL_NOT_FOUND, "unable to find '%s' in '%s'\n",
				name, elf_fn);
}

// vim:ts=4:sw=4

