/* 
 * scan_opcode.c
 */

#include <stdint.h>
#include "x86_opcode.h"

#include <stdio.h>
#include <stdlib.h>


extern const struct opcode_table_entry normal_insts[256];
extern const struct opcode_table_entry group1_insts[8];

/* if this instruction is jmp, then return NULL. if not, return
 * the address of next instruction */
uint8_t *
next_inst(uint8_t * stream)
{
	uint8_t prefix1 = 0;
	uint8_t prefix2 = 0;
	uint8_t prefix3 = 0;
	uint8_t prefix4 = 0;
	uint8_t opc;
	struct opcode_table_entry inn_e;
	const struct opcode_table_entry * e = NULL;
	uint8_t modrm = 0;

#define MOD(x)	(((x) & 0xc0)>>6)
#define REG(x)	(((x) & 0x38)>>3)
#define RM(x)	(((x) & 0x7))
restart:
	/* lookup in normal table */
	opc = *stream;
	stream ++;
	e = &normal_insts[opc];

	switch (e->type) {
		case INST_PREFIX1:
			prefix1 = opc;
			goto restart;
		case INST_PREFIX2:
			prefix2 = opc;
			goto restart;
		case INST_PREFIX3:
			prefix3 = opc;
			goto restart;
		case INST_PREFIX4:
			prefix4 = opc;
			goto restart;
		case INST_INVALID:
			printf("invalid instruction 0x%x\n", opc);
			exit(-1);
			break;
		case INST_NORMAL:
			break;
		case INST_GROUP1:
			modrm = *(stream);
			inn_e = *e;
			inn_e.name = group1_insts[REG(modrm)].name;
			e = &inn_e;
			break;
		case INST_GROUP1A:
		case INST_GROUP2:
		case INST_GROUP3:
		case INST_GROUP4:
		case INST_GROUP5:
		case INST_GROUP6:
		case INST_GROUP7:
		case INST_GROUP8:
		case INST_GROUP9:
		case INST_GROUP10:
		case INST_GROUP11:
		case INST_GROUP12:
		case INST_GROUP13:
		case INST_GROUP14:
		case INST_GROUP15:
		case INST_GROUP16:
		case INST_ESCAPE:
		case INST_SPECIAL:
		default:
			printf("type %d not implelented\n", e->type);
			exit(-1);
	}

	printf("instruction 0x%x:%s\n", opc, e->name);

	if (e->jmpnote != 0)
		return NULL;

	if (e->nr_operades == 0)
		return stream;

	/* modrm */
	uint32_t flag = 0;
	int operade_size = 4;
	int address_size = 4;
	/* prefix group 3 */
	if (prefix3 == 0x66)
		operade_size = 2;
	if (prefix4 == 0x67)
		address_size = 2;

	for (int i = 0; i < e->nr_operades; i++)
		flag |= e->operades[i].addressing;

	/* modrm and disp */
	if ((flag & REQ_MODRM) || 
		((e->type >= INST_GROUP1) && (e->type <= INST_GROUP16))) {
		modrm = *stream;
		stream ++;
		/* SIB */
		uint8_t mod_rm = (modrm & 0xc7);
		if (address_size == 2) {
			if (mod_rm == 0x06)
				stream += 2;
			else if (MOD(modrm) == 1)
				stream += 1;
			else if (MOD(modrm) == 2)
				stream += 2;
		} else {
			/* addressing == 4 (*8bit) */
			int disp = 0;
			int have_sib = 0;

			if ((RM(modrm) == 0x04) && (MOD(modrm) != 0x03))
				have_sib = 1;
			if (mod_rm == 0x05)
				disp = 4;
			if (MOD(modrm) == 0x01)
				disp = 1;
			else if (MOD(modrm) == 0x02)
				disp = 4;
			if (have_sib)
				stream += 1;
			stream += disp;
		}
	}

	/* operade */
	if (e->nr_operades == 0)
		return stream;
	for (int i = 0; i < e->nr_operades; i++) {
		int addressing = e->operades[i].addressing;
		switch (addressing) {
			case OP_ADDR_A:
				stream += 2 + operade_size;
				break;
			case OP_ADDR_I:
			case OP_ADDR_J:
				{
					uint32_t sz = e->operades[i].size;
					uint32_t base_sz = sz & 0xffff;
					uint32_t real_sz;
					real_sz = ((sz & RESPECT_OPERADE_SIZE_66) && prefix3) ?
						2 : base_sz;
					stream += real_sz;
				}
				break;
			case OP_ADDR_O:
				stream += address_size;
				break;
			case OP_ADDR_X:
			case OP_ADDR_Y:
				break;
			default:
				if (!((addressing & REQ_MODRM) ||
						(addressing & OPERADE_ADDRESSING_CONSTANT) ||
				   		(addressing & OPERADE_ADDRESSING_REGISTER)))  {
					printf("wrong addressing 0x%x\n", 
							addressing);
					exit(-1);
				}
		}
	}
	return stream;
}

// vim:ts=4:sw=4

