/* 
 * x86_instructions.tbl.h
 * by WN @ Jan. 18, 2010
 */

#ifndef X86_INSTRUCTIONS_TBL_H
#define X86_INSTRUCTIONS_TBL_H

enum _operator_type {
	_OPERATOR_NORMAL,
	_OPERATOR_SPECIAL,
} operator_type;

enum _operade_type {
	_OPERADE_NORMAL,
	_OPERADE_CONSTANT,
	_OPERADE_REGISTERS,
};

struct _regs {
	const char * r1;
	const char * r2;
};

struct _operade {
	enum _operade_type type;
	union {
		struct _regs regs;
		const char * normal;
		int constant;
	} u;
};

struct _hint {
	const char * hint;
};

struct _entry_rng {
	int start;
	int end;
};

extern void add_descriptor(
	const char * table,
	int head,
	const char * operator,
	struct _entry_rng rng,
	int nr_operades,
	struct _operade * operades,
	int nr_hints,
	struct _hint * hints,
	int jmpnote);

extern void print_table(void);

#endif
// vim:ts=4:sw=4

