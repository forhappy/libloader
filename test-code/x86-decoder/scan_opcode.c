/* 
 * scan_opcode.c
 */

#include <stdint.h>
#include "x86_opcode.h"

#include <stdio.h>
#include <stdlib.h>


extern struct opcode_table_entry normal_insts[256];

/* if this instruction is jmp, then return NULL. if not, return
 * the address of next instruction */
uint8_t *
next_inst(uint8_t * stream)
{
	uint32_t prefix = 0;
	struct opcode_table_entry * e = NULL;
restart:
	/* lookup in normal table */
	e = &normal_insts[*stream];

	switch (e->type) {
		case INST_PREFIX1:
		case INST_PREFIX2:
		case INST_PREFIX3:
		case INST_PREFIX4:
			prefix |= ((*stream) << ((e->type - INST_PREFIX1) * 2));
			stream ++;
			goto restart;
		case INST_INVALID:
			printf("invalid instruction 0x%x\n", *stream);
			exit(-1);
			break;
		case INST_GROUP1:
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

	if (e->jmpnote != 0)
		return NULL;

	/* modrm */
}

// vim:ts=4:sw=4

