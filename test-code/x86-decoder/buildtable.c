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
				operades[i].addressing = REQ_OPERADE_CONSTANT | __operades[i].u.constant;
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
		inst_table == twobytes_insts;
	else if (strcmp(table, "threebytes_0f38") == 0)
		inst_table == threebytes_0f38_insts;
	else if (strcmp(table, "threebytes_0f3a") == 0)
		inst_table == threebytes_0f3a_insts;
	else {
		printf("wrong table %s\n", table);
		exit(1);
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
				if (strcmp("GRP1", inst->name) == 0)
					inst->type = INST_GROUP1;
				else if (strcmp("GRP1A", inst->name) == 0)
					inst->type = INST_GROUP1A;
				else
					inst->type = INST_GROUP2 + atoi(&(inst->name[3])) - 2;
			} else if (strcmp("INVALID", inst->name) == 0) {
				inst->type = INST_INVALID;
			} else if (strcmp("BAD", inst->name) == 0) {
				inst->type = INST_INVALID;
			} else if (strcmp("ESCAPE", inst->name) == 0) {
				inst->type = INST_ESCAPE;
			} else if (strcmp("SPECIAL", inst->name) == 0) {
				inst->type = INST_SPECIAL;
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
print_one_table(const char * head, struct opcode_table_entry * table)
{
	printf("struct opcode_table_entry %s[256] = {\n", head);
	for (int i = 0; i < 256; i++) {
		struct opcode_table_entry inst = table[i];
		if (inst.type == INST_INVALID)
			continue;
		printf("\t[0x%x] = {\n", i);
		printf("\t\t.type = %d,\n", inst.type);
		printf("\t\t.name = \"%s\",\n", inst.name);
		printf("\t\t.nr_operades = %d,\n", inst.nr_operades);
		if (inst.nr_operades > 0) {
			printf("\t\t.operades = {\n");
			for (int j = 0; j < inst.nr_operades; j++) {
				printf("\t\t\t[%d] = {\n", j);
				printf("\t\t\t\t.addressing = 0x%x,\n", inst.operades[j].addressing);
				printf("\t\t\t\t.size = 0x%x,\n", inst.operades[j].size);
				if (inst.operades[j].addressing & OPERADE_ADDRESSING_REGISTER)
					printf("\t\t\t\t.reg_num = 0x%x,\n", inst.operades[j].reg_num);
				printf("\t\t\t},\n");
			}
			printf("\t\t},\n");
			if (inst.jmpnote)
				printf("\t\t.jmpnote = 1,\n");
		}
		printf("\t},\n");
		printf("\n");
	}
	printf("};\n");
	printf("\n");
}

void print_table(void)
{
	printf("#include \"x86_opcode.h\"\n");
	printf("// ----------------- begin print tables --------------\n");
	print_one_table("normal_insts",
			normal_insts);
	print_one_table("twobytes_insts",
			twobytes_insts);
	print_one_table("threebytes_0f38_insts",
			threebytes_0f38_insts);
	print_one_table("threebytes_0f3a_insts",
			threebytes_0f3a_insts);
	printf("// vim:ts=4:sw=4\n");
	return;
}


// vim:ts=4:sw=4

