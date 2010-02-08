/* 
 * x86_registers.h
 * by WN @ Jan. 07, 2010
 */
#ifndef X86_REGISTERS_H
#define X86_REGISTERS_H

struct x86_register
{
  char *reg_name;
  unsigned int reg_type;
  unsigned int reg_flags;
#define RegRex	    0x1  /* Extended register.  */
#define RegRex64    0x2  /* Extended 8 bit register.  */
  unsigned int reg_num;
}
reg_entry;

  /* register */
#define Reg8		   0x1	/* 8 bit reg */
#define Reg16		   0x2	/* 16 bit reg */
#define Reg32		   0x4	/* 32 bit reg */
#define Reg64		   0x8	/* 64 bit reg */
  /* immediate */
#define Imm8		  0x10	/* 8 bit immediate */
#define Imm8S		  0x20	/* 8 bit immediate sign extended */
#define Imm16		  0x40	/* 16 bit immediate */
#define Imm32		  0x80	/* 32 bit immediate */
#define Imm32S		 0x100	/* 32 bit immediate sign extended */
#define Imm64		 0x200	/* 64 bit immediate */
#define Imm1		 0x400	/* 1 bit immediate */
  /* memory */
#define BaseIndex	 0x800
  /* Disp8,16,32 are used in different ways, depending on the
     instruction.  For jumps, they specify the size of the PC relative
     displacement, for baseindex type instructions, they specify the
     size of the offset relative to the base register, and for memory
     offset instructions such as `mov 1234,%al' they specify the size of
     the offset relative to the segment base.  */
#define Disp8		0x1000	/* 8 bit displacement */
#define Disp16		0x2000	/* 16 bit displacement */
#define Disp32		0x4000	/* 32 bit displacement */
#define Disp32S	        0x8000	/* 32 bit signed displacement */
#define Disp64	       0x10000	/* 64 bit displacement */
  /* specials */
#define InOutPortReg   0x20000	/* register to hold in/out port addr = dx */
#define ShiftCount     0x40000	/* register to hold shift count = cl */
#define Control	       0x80000	/* Control register */
#define Debug	      0x100000	/* Debug register */
#define Test	      0x200000	/* Test register */
#define FloatReg      0x400000	/* Float register */
#define FloatAcc      0x800000	/* Float stack top %st(0) */
#define SReg2	     0x1000000	/* 2 bit segment register */
#define SReg3	     0x2000000	/* 3 bit segment register */
#define Acc	     0x4000000	/* Accumulator %al or %ax or %eax */
#define JumpAbsolute 0x8000000
#define RegMMX	    0x10000000	/* MMX register */
#define RegXMM	    0x20000000	/* XMM registers in PIII */
#define EsSeg	    0x40000000	/* String insn operand with fixed es segment */

  /* RegMem is for instructions with a modrm byte where the register
     destination operand should be encoded in the mod and regmem fields.
     Normally, it will be encoded in the reg field. We add a RegMem
     flag to the destination register operand to indicate that it should
     be encoded in the regmem field.  */
#define RegMem	    0x80000000

#define Reg	(Reg8|Reg16|Reg32|Reg64) /* gen'l register */
#define WordReg (Reg16|Reg32|Reg64)
#define ImplicitRegister (InOutPortReg|ShiftCount|Acc|FloatAcc)
#define Imm	(Imm8|Imm8S|Imm16|Imm32S|Imm32|Imm64) /* gen'l immediate */
#define EncImm	(Imm8|Imm16|Imm32|Imm32S) /* Encodable gen'l immediate */
#define Disp	(Disp8|Disp16|Disp32|Disp32S|Disp64) /* General displacement */
#define AnyMem	(Disp8|Disp16|Disp32|Disp32S|BaseIndex)	/* General memory */
  /* The following aliases are defined because the opcode table
     carefully specifies the allowed memory types for each instruction.
     At the moment we can only tell a memory reference size by the
     instruction suffix, so there's not much point in defining Mem8,
     Mem16, Mem32 and Mem64 opcode modifiers - We might as well just use
     the suffix directly to check memory operands.  */
#define LLongMem AnyMem		/* 64 bits (or more) */
#define LongMem AnyMem		/* 32 bit memory ref */
#define ShortMem AnyMem		/* 16 bit memory ref */
#define WordMem AnyMem		/* 16, 32 or 64 bit memory ref */
#define ByteMem AnyMem		/* 8 bit memory ref */

extern struct x86_register
find_register(const char * _name);

#endif

// vim:ts=4:sw=4

