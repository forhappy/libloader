/* 
 * asm/relocate.c
 * by WN @ Feb. 23, 2010
 */

#include <stdint.h>
#include <common/defs.h>
#include <common/assert.h>
#include <asm/syscall.h>
#include <asm/utils.h>

/* elf structures */

typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef int32_t		Elf32_Sword;
typedef uint32_t	Elf32_Word;

#define DT_NULL		0
#define DT_NEEDED	1
#define DT_PLTRELSZ	2
#define DT_PLTGOT	3
#define DT_HASH		4
#define DT_STRTAB	5
#define DT_SYMTAB	6
#define DT_RELA		7
#define DT_RELASZ	8
#define DT_RELAENT	9
#define DT_STRSZ	10
#define DT_SYMENT	11
#define DT_INIT		12
#define DT_FINI		13
#define DT_SONAME	14
#define DT_RPATH 	15
#define DT_SYMBOLIC	16
#define DT_REL	        17
#define DT_RELSZ	18
#define DT_RELENT	19
#define DT_PLTREL	20
#define DT_DEBUG	21
#define DT_TEXTREL	22
#define DT_JMPREL	23
#define DT_ENCODING	32
#define OLD_DT_LOOS	0x60000000
#define DT_LOOS		0x6000000d
#define DT_HIOS		0x6ffff000
#define DT_VALRNGLO	0x6ffffd00
#define DT_VALRNGHI	0x6ffffdff
#define DT_ADDRRNGLO	0x6ffffe00
#define DT_ADDRRNGHI	0x6ffffeff
#define DT_VERSYM	0x6ffffff0
#define DT_RELACOUNT	0x6ffffff9
#define DT_RELCOUNT	0x6ffffffa
#define DT_FLAGS_1	0x6ffffffb
#define DT_VERDEF	0x6ffffffc
#define	DT_VERDEFNUM	0x6ffffffd
#define DT_VERNEED	0x6ffffffe
#define	DT_VERNEEDNUM	0x6fffffff
#define OLD_DT_HIOS     0x6fffffff
#define DT_LOPROC	0x70000000
#define DT_HIPROC	0x7fffffff

#define ELF_ST_BIND(x)		((x) >> 4)
#define ELF_ST_TYPE(x)		(((unsigned int) x) & 0xf)
#define ELF32_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x)	ELF_ST_TYPE(x)
#define ELF32_R_SYM(x) ((x) >> 8)
#define ELF32_R_TYPE(x) ((x) & 0xff)

#define R_386_RELATIVE	   8		/* Adjust by program base */


struct elf32_dyn {
  Elf32_Sword d_tag;
  union{
    Elf32_Sword	d_val;
    Elf32_Addr	d_ptr;
  } d_un;
};

struct elf32_rel {
  Elf32_Addr	r_offset;
  Elf32_Word	r_info;
};

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
			default:
				printf("unknown relocate type 0x%x\n", type);
				__exit(-1);
				break;
		}
		*(uint32_t*)(address) = real_val;
	}
}

// vim:ts=4:sw=4

