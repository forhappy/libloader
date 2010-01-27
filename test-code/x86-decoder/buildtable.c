/* 
 * buildtable.c
 * by WN @ Jan. 18. 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "x86_instructions.tbl.h"
#include "x86_opcode.h"
#include "x86_registers.h"

static struct opcode_table_entry normal_insts[256];
static struct opcode_table_entry twobytes_insts[256];
static struct opcode_table_entry threebytes_0f38_insts[256];
static struct opcode_table_entry threebytes_0f3a_insts[256];

static struct group_table_name_map_entry {
	const char * string;
} group_table_name_map[NR_GROUPS] = {
	[TABLE_GROUP1_0x80] = "GRP1_0x80",
	[TABLE_GROUP1_0x81] = "GRP1_0x81",
	[TABLE_GROUP1_0x82] = "GRP1_0x82",
	[TABLE_GROUP1_0x83] = "GRP1_0x83",
	[TABLE_GROUP1A_0x8f] = "GRP1A_0x8f",
	[TABLE_GROUP2_0xc0] = "GRP2_0xc0",
	[TABLE_GROUP2_0xc1] = "GRP2_0xc1",
	[TABLE_GROUP2_0xd0] = "GRP2_0xd0",
	[TABLE_GROUP2_0xd1] = "GRP2_0xd1",
	[TABLE_GROUP2_0xd2] = "GRP2_0xd2",
	[TABLE_GROUP2_0xd3] = "GRP2_0xd3",
	[TABLE_GROUP3_0xf6] = "GRP3_0xf6",
	[TABLE_GROUP3_0xf7] = "GRP3_0xf7",
	[TABLE_GROUP4_0xfe] = "GRP4_0xfe",
	[TABLE_GROUP5_0xff] = "GRP5_0xff",
	[TABLE_GROUP6_0x0f00] = "GRP6_0x0f00",
	[TABLE_GROUP7_0x0f01_mem] = "GRP7_0x0f01_mem",
	[TABLE_GROUP7_0x0f01_11b] = "GRP7_0x0f01_11b",
	[TABLE_GROUP8_0x0fba] = "GRP8_0x0fba",
	[TABLE_GROUP9_0x0fc7] = "GRP9_0x0fc7",
	[TABLE_GROUP10_0x0fb9] = "GRP10_0f89",
	[TABLE_GROUP11_0xc6] = "GRP11_0xc6",
	[TABLE_GROUP11_0xc7] = "GRP11_0xc7",
	[TABLE_GROUP12_0x0f71] = "GRP12_0x0f71",
	[TABLE_GROUP13_0x0f72] = "GRP13_0x0f72",
	[TABLE_GROUP14_0x0f73] = "GRP14_0x0f73",
	[TABLE_GROUP15_0x0fae] = "GRP15_0x0fae",
	[TABLE_GROUP16_0x0f18] = "GRP16_0x0f18",
};

static struct opcode_table_entry groups_table[NR_GROUPS][8];


static void
set_normal_operade(struct operade * d, struct _operade * s)
{
	d->addressing = 0;
	d->size = 0;
	switch (s->u.normal[0]) {
		case 'A':
			d->addressing = OP_ADDR_A;
			break;
		case 'C':
			d->addressing = OP_ADDR_C;
			break;
		case 'D':
			d->addressing = OP_ADDR_D;
			break;
		case 'E':
			d->addressing = OP_ADDR_E;
			break;
		case 'F':
			d->addressing = OP_ADDR_F;
			break;
		case 'G':
			d->addressing = OP_ADDR_G;
			break;
		case 'I':
			d->addressing = OP_ADDR_I;
			break;
		case 'J':
			d->addressing = OP_ADDR_J;
			break;
		case 'M':
			d->addressing = OP_ADDR_M;
			break;
		case 'N':
			d->addressing = OP_ADDR_N;
			break;
		case 'O':
			d->addressing = OP_ADDR_O;
			break;
		case 'P':
			d->addressing = OP_ADDR_P;
			break;
		case 'Q':
			d->addressing = OP_ADDR_Q;
			break;
		case 'R':
			d->addressing = OP_ADDR_R;
			break;
		case 'S':
			d->addressing = OP_ADDR_S;
			break;
		case 'U':
			d->addressing = OP_ADDR_U;
			break;
		case 'V':
			d->addressing = OP_ADDR_V;
			break;
		case 'W':
			d->addressing = OP_ADDR_W;
			break;
		case 'Y':
			d->addressing = OP_ADDR_Y;
			break;
		case 'X':
			d->addressing = OP_ADDR_X;
			break;
		default:
			printf("wrong addressing: %s\n", s->u.normal);
			exit(-1);
	}

	const char * str_sz = strdupa(&s->u.normal[1]);

	if (strcmp("a", str_sz) == 0)
		d->size = OP_SIZE_a;
	else if (strcmp("b", str_sz) == 0)
		d->size = OP_SIZE_b;
	else if (strcmp("c", str_sz) == 0)
		d->size = OP_SIZE_c;
	else if (strcmp("d", str_sz) == 0)
		d->size = OP_SIZE_d;
	else if (strcmp("dq", str_sz) == 0)
		d->size = OP_SIZE_dq;
	else if (strcmp("p", str_sz) == 0)
		d->size = OP_SIZE_p;
	else if (strcmp("pd", str_sz) == 0)
		d->size = OP_SIZE_pd;
	else if (strcmp("pi", str_sz) == 0)
		d->size = OP_SIZE_pi;
	else if (strcmp("ps", str_sz) == 0)
		d->size = OP_SIZE_ps;
	else if (strcmp("q", str_sz) == 0)
		d->size = OP_SIZE_q;
	else if (strcmp("s", str_sz) == 0)
		d->size = OP_SIZE_s;
	else if (strcmp("ss", str_sz) == 0)
		d->size = OP_SIZE_ss;
	else if (strcmp("si", str_sz) == 0)
		d->size = OP_SIZE_si;
	else if (strcmp("v", str_sz) == 0)
		d->size = OP_SIZE_v;
	else if (strcmp("w", str_sz) == 0)
		d->size = OP_SIZE_w;
	else if (strcmp("z", str_sz) == 0)
		d->size = OP_SIZE_z;
	else if (strcmp("xx", str_sz) == 0)
		d->size = OP_SIZE_0;
	else {
		printf("wrong operade size: %s\n", s->u.normal);
		exit(-1);
	}
	return;
}

static void
set_register_operade(struct operade * d, struct _operade * s)
{
	struct x86_register reg = find_register(s->u.regs.r1);

	d->addressing = OPERADE_ADDRESSING_REGISTER;
	d->reg_num = reg.reg_num;
	if (s->u.regs.r2 != NULL)
		d->addressing |= OPERADE_ADDRESSING_REGISTER_RESPECT_REXRB;
	/* extended 8 bits register */
	if (reg.reg_flags & RegRex64)
		d->addressing |= OPERADE_REGISTER_REXEXT8BIT;

	if (reg.reg_type & Reg8)
		d->size = 1;
	if (reg.reg_type & Reg16)
		d->size = 2;
	if (reg.reg_type & Reg32)
		d->size = 4;
	if (reg.reg_type & Reg64)
		d->size = 8;

	d->size |= OPERADE_REGISTER;

	if (s->u.regs.r1[0] == 'e')
		d->size |= OPERADE_REGISTER_RESPECT_66;
	if (s->u.regs.r1[0] == 'r')
		d->size |= OPERADE_REGISTER_RESPECT_REXW | OPERADE_REGISTER_RESPECT_66;
}


void add_descriptor(
		const char * table,
		int head,
		const char * operator,
		struct _entry_rng rng,
		int nr_operades,
		struct _operade * __operades,
		int nr_hints,
		struct _hint * __hints,
		int jmpnote)
{

	/* if it is a x86_64 only instruction, omit */
	if (head == 64)
		return;

	struct operade operades[4];
	struct _hint hints[4];

	for (int i = 0; i < nr_operades; i++) {
		switch (__operades[i].type) {
			case _OPERADE_NORMAL:
				set_normal_operade(&operades[i], &__operades[i]);
				break;
			case _OPERADE_CONSTANT:
				operades[i].addressing = OPERADE_ADDRESSING_CONSTANT | __operades[i].u.constant;
				break;
			case _OPERADE_REGISTERS:
				set_register_operade(&operades[i], &__operades[i]);
				break;
			default:
				printf("unknown operade type %d\n", __operades[i].type);
				exit(-1);
				break;
		}
	}

	for (int i = 0; i < nr_hints; i++) {
		hints[i] = __hints[i];
		if (strcmp(hints[i].hint, "o64") == 0)
			return;
	}

	struct opcode_table_entry * inst_table = NULL;
	if (strcmp(table, "onebyte") == 0)
		inst_table = normal_insts;
	else if (strcmp(table, "twobytes") == 0)
		inst_table = twobytes_insts;
	else if (strcmp(table, "threebytes_0f38") == 0)
		inst_table = threebytes_0f38_insts;
	else if (strcmp(table, "threebytes_0f3a") == 0)
		inst_table = threebytes_0f3a_insts;
	else {
		/* find the table */
		int i;
		for (i = 0; i < NR_GROUPS; i++) {
			if (strcmp(table, group_table_name_map[i].string) == 0)
				break;
		}
		if (i >= NR_GROUPS) {
			printf("wrong table %s\n", table);
			exit(-1);
		}
		inst_table = (groups_table[i]);
	}

	for (int i = rng.start; i <= rng.end; i++) {
		struct opcode_table_entry * inst = &inst_table[i];

		inst->name = strdup(operator);
		inst->nr_operades = nr_operades;
		/* decide the operator name */
		if (islower(inst->name[0])) {
			inst->type = INST_NORMAL;
		} else {
			if (strncmp("PREFIX", inst->name, 6) == 0) {
				inst->type = INST_PREFIX1 + inst->name[6] - '1';
			} else if (strncmp("GRP", inst->name, 3) == 0) {
				/* find group */
				int i;
				for (i = 0; i < NR_GROUPS; i++) {
					if (strcmp(inst->name, group_table_name_map[i].string) == 0)
						break;
				}
				if (i >= NR_GROUPS) {
					printf("wrong group %s\n", inst->name);
					exit(-1);
				}
				inst->type = INST_GROUP_start + 1 + i;
			} else if (strcmp("INVALID", inst->name) == 0) {
				inst->type = INST_INVALID;
			} else if (strcmp("BAD", inst->name) == 0) {
				inst->type = INST_INVALID;
			} else if (strcmp("ESCAPETWOBYTES", inst->name) == 0) {
				inst->type = INST_ESCAPE_2B;
			} else if (strcmp("ESCAPETHREEBYTES", inst->name) == 0) {
				inst->type = INST_ESCAPE_3B;
			} else if (strcmp("ESCAPECOPROCESSOR", inst->name) == 0) {
				inst->type = INST_ESCAPE_COP;
			} else if (strcmp("SPECIAL", inst->name) == 0) {
				inst->type = INST_SPECIAL;
			} else if (strcmp("XXXGRP7", inst->name) == 0) {
				inst->type = INST_GROUP7;
			} else {
				printf("unknow type %s\n", inst->name);
				exit(1);
			}
		}

		/* operades */
		for (int i = 0; i < nr_operades; i++)
			inst->operades[i] = operades[i];

		/* jmpnote */
		if (jmpnote)
			inst->jmpnote = 1;
	}
	return;
}

static void
print_one_table(const char * head, struct opcode_table_entry * table,
		int nr_opc)
{
	if (nr_opc == 256)
		printf("struct opcode_table_entry %s[%d] = {\n", head, nr_opc);

	for (int i = 0; i < nr_opc; i++) {
		struct opcode_table_entry inst = table[i];
		if (inst.type == INST_INVALID)
			continue;
		int flag = 0;
		printf("\t[0x%x] = {\n", i);
		printf("\t\t.type = %d,\n", inst.type);
		printf("\t\t.name = \"%s\",\n", inst.name);
		printf("\t\t.nr_operades = %d,\n", inst.nr_operades);
		if (inst.nr_operades > 0) {
			printf("\t\t.operades = {\n");
			for (int j = 0; j < inst.nr_operades; j++) {
				printf("\t\t\t[%d] = {\n", j);
				printf("\t\t\t\t.addressing = 0x%x,\n", inst.operades[j].addressing);
				flag |= inst.operades[j].addressing;
				printf("\t\t\t\t.size = 0x%x,\n", inst.operades[j].size);
				if (inst.operades[j].addressing & OPERADE_ADDRESSING_REGISTER)
					printf("\t\t\t\t.reg_num = 0x%x,\n", inst.operades[j].reg_num);
				printf("\t\t\t},\n");
			}
			printf("\t\t},\n");
		}
		printf("\t\t.req_modrm = %d,\n", (flag & REQ_MODRM) ? 1 : 0);
		if (inst.jmpnote)
			printf("\t\t.jmpnote = 1,\n");
		printf("\t},\n");
		printf("\n");
	}
	if (nr_opc == 256)
		printf("};\n");
	printf("\n");
}

void print_table(void)
{
	printf("#include \"x86_opcode.h\"\n");
	printf("// ----------------- begin print tables --------------\n");
	print_one_table("normal_insts",
			normal_insts, 256);
	print_one_table("twobytes_insts",
			twobytes_insts, 256);
	print_one_table("threebytes_0f38_insts",
			threebytes_0f38_insts, 256);
	print_one_table("threebytes_0f3a_insts",
			threebytes_0f3a_insts, 256);
	printf("struct opcode_table_entry group_insts[%d][8] = {\n", NR_GROUPS);
	for (int i = 0; i < NR_GROUPS; i++) {
		printf("\t[%d] = {\n", i);
		print_one_table(NULL, groups_table[i], 8);
		printf("\t},\n");
	}
	printf("};\n");
	printf("// vim:ts=4:sw=4\n");
	return;
}


// vim:ts=4:sw=4

