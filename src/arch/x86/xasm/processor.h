#ifndef __X86_PROCESSOR_H
#define __X86_PROCESSOR_H

/* xmain return stack adjustment value, in dwords */
struct pusha_regs {
	uint32_t flags;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
};

#define stack_top(r)	((r)->esp)

#define GDT_ENTRY_TLS_MIN	6
#define GDT_ENTRY_TLS_ENTRIES 3
#define GDT_ENTRY_TLS_MAX 	(GDT_ENTRY_TLS_MIN + GDT_ENTRY_TLS_ENTRIES - 1)

struct user_desc {
	unsigned int  entry_number;
	unsigned int  base_addr;
	unsigned int  limit;
	unsigned int  seg_32bit:1;
	unsigned int  contents:2;
	unsigned int  read_exec_only:1;
	unsigned int  limit_in_pages:1;
	unsigned int  seg_not_present:1;
	unsigned int  useable:1;
};

struct i387_fxsave_struct {
	uint16_t	cwd;
	uint16_t	swd;
	uint16_t	twd;
	uint16_t	fop;
	union {
		struct {
			uint64_t	rip;
			uint64_t	rdp;
		};
		struct {
			uint32_t	fip;
			uint32_t	fcs;
			uint32_t	foo;
			uint32_t	fos;
		};
	};
	uint32_t	mxcsr;
	uint32_t	mxcsr_mask;
	uint32_t	st_space[32];	/* 8*16 bytes for each FP-reg = 128 bytes */
	uint32_t	xmm_space[64];	/* 16*16 bytes for each XMM-reg = 256 bytes */
	uint32_t	padding[24];
};

#endif

// vim:ts=4:sw=4

