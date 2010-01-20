#ifndef X86_INSTRUCTION_H
#define X86_INSTRUCTION_H

enum x86_instruction_type {
	X86INST_PREFIX66 = 0x66,
};

struct x86_instruction_t {
	const char * name;
	enum x86_instruction_type type;
	/* 4 groups of prefix, at least one for each group */
	uint32_t prefix;
	/* useless in x86_32 */
	uint8_t rex;
	uint8_t opcode;
	uint8_t modrm;
	uint8_t sib;
	/* disp follow modrm */
	uint32_t disp;
	uint32_t imm;
	/* the additional operand */
	uint32_t additional;
	/* the total length of the instruction */
	int length;
};

#endif

// vim:ts=4:sw=4

