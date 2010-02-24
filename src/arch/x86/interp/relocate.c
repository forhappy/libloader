/* 
 * asm/relocate.c
 * by WN @ Feb. 23, 2010
 */

#include <stdint.h>
#include <common/defs.h>
#include <common/assert.h>
#include <asm/syscall.h>
#include <asm/utils.h>
#include <asm/elf.h>

/* see glibc code: sysdeps/i386/elf/configure.in: 
 * 
 * dnl It is always possible to access static and hidden symbols in an
 * dnl position independent way.
 * AC_DEFINE(PI_STATIC_AND_HIDDEN)
 *
 * also see glibc's dl-machine.h
 *
 * %ebx is _GLOBAL_OFFSET_TABLE_
 * _GLOBAL_OFFSET_TABLE_[0] is link time address _DYNAMIC;
 * */

extern struct elf32_dyn _DYNAMIC[] ATTR_HIDDEN;
extern void* _GLOBAL_OFFSET_TABLE_[] ATTR_HIDDEN;

void
relocate_interp(void)
{
	uint32_t load_bias = (void*)(_DYNAMIC) - _GLOBAL_OFFSET_TABLE_[0];

	/* prepare relocation */
	struct elf32_rel * reltab = NULL;
	struct elf32_sym * symtab = NULL;
	int relsz = 0;
	int relent = 0;
	for (struct elf32_dyn * dyn = &_DYNAMIC[0]; dyn->d_tag != DT_NULL; dyn ++) {
		switch (dyn->d_tag) {
			case DT_RELSZ:
				relsz = dyn->d_un.d_val;
				break;
			case DT_REL:
				reltab = (void*)dyn->d_un.d_ptr + load_bias;
				break;
			case DT_RELENT:
				relent = dyn->d_un.d_val;
				break;
			case DT_SYMTAB:
				symtab = (void*)dyn->d_un.d_ptr + load_bias;
				break;
		}
		
	}
	
	assert(reltab != NULL);
	assert(relsz != 0);
	assert(relent != 0);
	int nr_rels = relsz / relent;

	for (int i = 0; i < nr_rels; i++) {
		uint32_t address;
		uint32_t info;

		address = reltab[i].r_offset + load_bias;
		info = reltab[i].r_info;

		int type = ELF32_R_TYPE(info);

		uint32_t real_val;
		switch (type) {
			case R_386_RELATIVE: {
				uint32_t old_val = *(uint32_t*)(address);
				real_val = old_val + load_bias;
				break;
			}
			case R_386_GLOB_DAT: {
				assert(symtab != NULL);
				int nsym = ELF32_R_SYM(info);
				struct elf32_sym * sym = &symtab[nsym];
				uint32_t sym_val = sym->st_value;
				real_val = sym_val + load_bias;
				break;
			}
			default:
				printf("relocate error: unknown relocate type 0x%x\n", type);
				__exit(-1);
				break;
		}
		*(uint32_t*)(address) = real_val;
	}
}

// vim:ts=4:sw=4

