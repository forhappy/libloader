#include <bfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
	bfd_init();

	char ** matching;
	bfd * file = bfd_openr("/home/wn/work/snitchaser/build/src/interp/libinterp.so", NULL);
	assert(file != NULL);
	if (!bfd_check_format_matches(file, bfd_object, &matching)) {
		bfd_close(file);
		return 0;
	}
	bfd_set_start_address(file, 0x10000000);
	int flags = bfd_get_file_flags(file);
	printf("%x\n", flags);
	assert(flags & HAS_SYMS);

	void * minisyms;
	unsigned int size;
	int symcount = bfd_read_minisymbols(file, 0, &minisyms, &size);
	printf("%d\n", symcount);
	assert(symcount >= 0);

	asymbol * store = bfd_make_empty_symbol(file);
	assert(store != NULL);

	bfd_byte * from, * fromend, * to;
	from = (bfd_byte *) minisyms;
	fromend = from + symcount * size;
	to = (bfd_byte *)minisyms;

	for (; from < fromend; from += size) {
		asymbol * sym;
		sym = bfd_minisymbol_to_symbol(file, 0, (const void *)from,
				store);

		symbol_info syminfo;
		bfd_get_symbol_info(file, sym, &syminfo);
		printf("0x%llx, %s\n", syminfo.value, sym->name);
	}

	bfd_close(file);
	return 0;
}

