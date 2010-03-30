/* 
 * auxv.c
 * by WN @ Mar. 03, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>
#include <interp/auxv.h>
#include <common/linux/auxvec.h>
#include <xasm/elf.h>

struct auxv_info auxv_info = {
	.p_user_entry = NULL,
	.ppuser_phdrs = NULL,
	.p_nr_user_phdrs = NULL,
	.p_base = NULL,
	.p_execfn = NULL,
	.p_sysinfo = NULL,
	.p_sysinfo_ehdr = NULL,
};

void
print_auxv(void)
{
	DEBUG(LOADER, "AT_ENTRY=%p\n", *auxv_info.p_user_entry);
	DEBUG(LOADER, "AT_BASE=%p\n", *auxv_info.p_base);
	DEBUG(LOADER, "AT_PHDR=%p\n", *auxv_info.ppuser_phdrs);
	DEBUG(LOADER, "AT_PHNUM=%d\n", *auxv_info.p_nr_user_phdrs);
	DEBUG(LOADER, "AT_EXECFN=%s\n", *auxv_info.p_execfn);
	DEBUG(LOADER, "AT_SYSINFO=%p\n", *auxv_info.p_sysinfo);
	DEBUG(LOADER, "AT_SYSINFO_EHDR=%p\n", *auxv_info.p_sysinfo_ehdr);
}

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
	VERBOSE(LOADER, "auxv start from %p\n", p);
	while (*p != AT_NULL) {
		switch (p[0]) {
			case AT_ENTRY: {
				auxv_info.p_user_entry = (void*)(&p[1]);
				break;
			}
			case AT_BASE: {
				auxv_info.p_base = (void*)&p[1];
				break;
			}
			case AT_PHDR: {
				auxv_info.ppuser_phdrs = (void*)(&p[1]);
				break;
			}
			case AT_PHENT: {
				assert(p[1] == sizeof(struct elf32_phdr));
				break;
			}
			case AT_PHNUM: {
				auxv_info.p_nr_user_phdrs = (void*)(&p[1]);
				break;
			}
			case AT_EXECFN: {
				auxv_info.p_execfn = (const char **)(&p[1]);
				break;
			}
			case AT_SYSINFO: {
				auxv_info.p_sysinfo = (void*)(&p[1]);
			}
			case AT_SYSINFO_EHDR: {
				auxv_info.p_sysinfo_ehdr = (struct elf32_phdr **)(&p[1]);
			}
			default:
				break;
		}
		p += 2;
	}
}

// vim:ts=4:sw=4

