/* 
 * auxv.c
 * by WN @ Mar. 03, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>
#include <interp/auxv.h>
#include <linux/auxvec.h>
#include <asm/elf.h>

struct auxv_info auxv_info = {
	.p_user_entry = NULL,
	.ppuser_phdrs = NULL,
	.p_nr_user_phdrs = NULL,
	.p_base = NULL,
	.p_execfn = NULL,
};

void
find_auxv(void * oldesp)
{
	/* find the aux vector */
	uintptr_t * p = oldesp;
	p++;
	while (*p != 0)
		p ++;
	p ++;
	while (*p != 0)
		p ++;
	p ++;

	/* now p is pointed to aux vector */
	TRACE(LOADER, "auxv start from %p\n", p);
	while (*p != AT_NULL) {
		switch (p[0]) {
			case AT_ENTRY: {
				auxv_info.p_user_entry = (void*)(&p[1]);
				SILENT(LOADER, "user entry = %p\n", *p_user_entry);
				break;
			}
			case AT_BASE: {
				auxv_info.p_base = (void*)&p[1];
				SILENT(LOADER, "interp base = %p\n", *p_base);
				break;
			}
			case AT_PHDR: {
				auxv_info.ppuser_phdrs = (void*)(&p[1]);
				SILENT(LOADER, "user phdr = %p\n", *ppuser_phdrs);
				break;
			}
			case AT_PHENT: {
				assert(p[1] == sizeof(struct elf32_phdr));
				break;
			}
			case AT_PHNUM: {
				auxv_info.p_nr_user_phdrs = (void*)(&p[1]);
				SILENT(LOADER, "phnum = %d\n", *p_nr_user_phdrs);
				break;
			}
			case AT_EXECFN: {
				auxv_info.p_execfn = (const char **)(&p[1]);
				SILENT(LOADER, "AT_EXECFN=%s\n", *p_execfn);
				break;
			}
			default:
				break;
		}
		p += 2;
	}
}

// vim:ts=4:sw=4

