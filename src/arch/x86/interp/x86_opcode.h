/* 
 * x86_opcode.h
 * by WN @ Jan. 20, 2010
 */

#ifndef X86_OPCODE_H
#define X86_OPCODE_H

enum group_table_name {
	TABLE_GROUP1_0x80,
	TABLE_GROUP1_0x81,
	TABLE_GROUP1_0x82,
	TABLE_GROUP1_0x83,
	TABLE_GROUP1A_0x8f,
	TABLE_GROUP2_0xc0,
	TABLE_GROUP2_0xc1,
	TABLE_GROUP2_0xd0,
	TABLE_GROUP2_0xd1,
	TABLE_GROUP2_0xd2,
	TABLE_GROUP2_0xd3,
	TABLE_GROUP3_0xf6,
	TABLE_GROUP3_0xf7,
	TABLE_GROUP4_0xfe,
	TABLE_GROUP5_0xff,
	TABLE_GROUP6_0x0f00,
	TABLE_GROUP7_0x0f01_mem,
	TABLE_GROUP7_0x0f01_11b,
	TABLE_GROUP8_0x0fba,
	TABLE_GROUP9_0x0fc7,
	TABLE_GROUP10_0x0fb9,
	TABLE_GROUP11_0xc6,
	TABLE_GROUP11_0xc7,
	TABLE_GROUP12_0x0f71,
	TABLE_GROUP13_0x0f72,
	TABLE_GROUP14_0x0f73,
	TABLE_GROUP15_0x0fae,
	TABLE_GROUP16_0x0f18,
	NR_GROUPS,
};

enum inst_type {
	INST_INVALID = 0,
	INST_NORMAL,
	INST_PREFIX1,
	INST_PREFIX2,
	INST_PREFIX3,
	INST_PREFIX4,
	INST_GROUP_start,
	INST_GROUP1_0x80,
	INST_GROUP1_0x81,
	INST_GROUP1_0x82,
	INST_GROUP1_0x83,
	INST_GROUP1A_0x8f,
	INST_GROUP2_0xc0,
	INST_GROUP2_0xc1,
	INST_GROUP2_0xd0,
	INST_GROUP2_0xd1,
	INST_GROUP2_0xd2,
	INST_GROUP2_0xd3,
	INST_GROUP3_0xf6,
	INST_GROUP3_0xf7,
	INST_GROUP4_0xfe,
	INST_GROUP5_0xff,
	INST_GROUP6_0x0f00,
	INST_GROUP7_0x0f01_mem,
	INST_GROUP7_0x0f01_11b,
	INST_GROUP8_0x0fba,
	INST_GROUP9_0x0fc7,
	INST_GROUP10_0x0fb9,
	INST_GROUP11_0xc6,
	INST_GROUP11_0xc7,
	INST_GROUP12_0x0f71,
	INST_GROUP13_0x0f72,
	INST_GROUP14_0x0f73,
	INST_GROUP15_0x0fae,
	INST_GROUP16_0x0f18,
	INST_GROUP_end,
	/* group 7 is defferent from other groups */
	INST_GROUP7,
	INST_ESCAPE_2B,
	INST_ESCAPE_3B_0x38,
	INST_ESCAPE_3B_0x3a,
	INST_ESCAPE_COP,
	INST_NEED_SPECPREFIX,
	INST_SPECIAL,
};


#define OPERADE_ADDRESSING_BASE_MASK	((1UL<<5)-1)
#define REQ_MODRM	(1<<8)
#define NON_MODRM	(0)
#define OPERADE_ADDRESSING_CONSTANT (1<<9)
#define OPERADE_ADDRESSING_REGISTER (1<<10)
#define OPERADE_ADDRESSING_REGISTER_RESPECT_REXRB (1<<11)

enum operade_addressing {
	OP_ADDR_A = NON_MODRM | 0,		/* Direct address, seems that only lcall(0x9a) and ljmp(0xea) use A */
	OP_ADDR_C = REQ_MODRM | 1,		/* have modrm, reg select control register */
	OP_ADDR_D = REQ_MODRM | 2,		/* have modrm, reg select debug register */
	OP_ADDR_E = REQ_MODRM | 3,		/* have modrm, most common */
	OP_ADDR_F = NON_MODRM | 4,		/* EFLAGS/RFLAGS register, without modrm */
	OP_ADDR_G = REQ_MODRM | 5,		/* have modrm, reg select a general register */
	OP_ADDR_I = NON_MODRM | 6,		/* imm data */
	OP_ADDR_J = NON_MODRM | 7,		/* jump offset */
	OP_ADDR_M = REQ_MODRM | 8,		/* have modrm, refer only to memory (lds) */
	OP_ADDR_N = REQ_MODRM | 9,		/* modrm R/M field selects a quadword (64bit) mmx register */
	OP_ADDR_O = NON_MODRM | 10,		/* no modrm, operade is a word or double word */
	OP_ADDR_P = REQ_MODRM | 11,		/* modrm reg field select a quadword MMX register */
	OP_ADDR_Q = REQ_MODRM | 12,		/* have modrm, R/M field selects a memory address or a 64bit MMX register */
	OP_ADDR_R = REQ_MODRM | 13,		/* have modrm, R/M field may refer only to a general register */
	OP_ADDR_S = REQ_MODRM | 14,		/* have modrm, reg field selects a segment register */
	OP_ADDR_U = REQ_MODRM | 15,		/* modrm R/M field selects select a 128bit XMM register */
	OP_ADDR_V = REQ_MODRM | 16,		/* modrm reg field selects a 128bit XMM register */
	OP_ADDR_W = REQ_MODRM | 17,		/* modrm R/M field selects a 128bit XMM register  */
	OP_ADDR_X = NON_MODRM | 18,		/* memory, addressed by DS:rSI (movs, cmps ...) */
	OP_ADDR_Y = NON_MODRM | 19,		/* memory, addredded by ES:rDI (movs, cmps ...)  */
};

#define OPERADE_SIZE_BASE_MASK	(0xffff)

#define IGN_OPERADE_SIZE	(0)
#define OPERADE_FLOAT		(1<<17)
#define OPERADE_SINGLEP		((1<<17) | (1<<18))
#define OPERADE_DOUBLEP		((1<<17) | (1<<19))
#define OPERADE_MMX_REGISTER	(1<<20)
#define OPERADE_XMM_REGISTER	(1<<21)
#define OPERADE_POINTER			(1<<22)
#define OPERADE_SCALAR			(1<<23)
#define OPERADE_REGISTER	(1<<24)
#define RESPECT_OPERADE_SIZE_66		(1<<25)
#define RESPECT_OPERADE_SIZE_REXW	(1<<26)
#define RESPECT_OPERADE_SIZE	(RESPECT_OPERADE_SIZE_66 | RESPECT_OPERADE_SIZE_REXW)
#define REQ_OPERADE_CONSTANT	(1<<27)

#define OPERADE_REGISTER_RESPECT_66 (1<<28)
/* extend 8 bit registers */
#define OPERADE_REGISTER_REXEXT8BIT (1<<29)
#define OPERADE_REGISTER_RESPECT_REXW (1<<30)

enum operade_size {
	OP_SIZE_a	= RESPECT_OPERADE_SIZE | 0x0000ffff,
										/* tow one-word operades in memory or two double-word
				  						    operades in memory, only by BOUND instruction */
	OP_SIZE_0	= 0,					/* doesn't really read the operade, only take its address, such as lea */
	OP_SIZE_b	= IGN_OPERADE_SIZE | 1,		/* byte */
	/* Very strange: seems no operator respect type of 'c' */
	OP_SIZE_c	= RESPECT_OPERADE_SIZE | 1,		/* byte or word */
	OP_SIZE_d	= IGN_OPERADE_SIZE | 4,		/* double word */
	OP_SIZE_dq	= IGN_OPERADE_SIZE | 128, 	/* double quadword */
	OP_SIZE_p	= RESPECT_OPERADE_SIZE | OPERADE_POINTER | 2,
											/* 32b, 48b or 80b pointer,  */
	OP_SIZE_pd	= IGN_OPERADE_SIZE | OPERADE_DOUBLEP | 16,
											/* 128bit packed double-precision floating
												point data*/
	OP_SIZE_pi	= IGN_OPERADE_SIZE | OPERADE_MMX_REGISTER | 8,
											/* quadword MMX */
	OP_SIZE_ps	= IGN_OPERADE_SIZE | OPERADE_SINGLEP | 16,
											/* 128bit packed single float */
	OP_SIZE_q	= IGN_OPERADE_SIZE | 8,		/* quadword */
	OP_SIZE_s	= 0xffff,					/* special: 6 byte or 10 byte pseudo-descriptor */
	OP_SIZE_ss	= IGN_OPERADE_SIZE | OPERADE_SCALAR | 16,
											/* scalar element of a 128 bit packed
											 * single-precision floating data */
	OP_SIZE_si	= IGN_OPERADE_SIZE | OPERADE_REGISTER | 4,
											/* double word register */
	OP_SIZE_v	= RESPECT_OPERADE_SIZE | 4,		/* word, douleword or quadword */
	OP_SIZE_w	= IGN_OPERADE_SIZE | 2,		/* word */
	OP_SIZE_z	= RESPECT_OPERADE_SIZE_66 | 4,		/* word for 16bit operade size,
											   double word for 32 or 64 bit operade size */
	OP_SIZE_sd	= IGN_OPERADE_SIZE | OPERADE_SCALAR | 8,	/* double-precision floating data
																see movsd */
};

struct operade {
	enum operade_addressing addressing;
	enum operade_size size;
	int reg_num;
};

enum spec_prefix_number {
	SPECPREFIX_NONE = 0,
	SPECPREFIX_0x66 = 1,
	SPECPREFIX_0xf2 = 2,
	SPECPREFIX_0xf3 = 3,
};

struct opcode_table_entry {
	enum inst_type type;
	const char * name;
	int nr_operades;
	struct operade operades[4];
	int req_modrm;
	int jmpnote;
};


#endif

// vim:ts=4:sw=4

