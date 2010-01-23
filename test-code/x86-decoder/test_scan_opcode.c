#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


extern uint8_t *
next_inst(uint8_t * stream);

static void func(void)
{
	asm volatile (
			"addb %bl, %ch\n"
			"addb %ch, 0x12345678\n"
			"addb %bh, 0x12345(%ecx, %ebx, 2)\n"
			"addr16 addb %ch, 0x1234\n"
			"addr16 addb %ch, (%bx, %si)\n"
			"addr16 addb %ch, 0x1234(%bx, %si)\n"

			"addw %bx, %cx\n"
			"addw %bx, 0x12345678\n"
			"addw %bx, (%ecx)\n"
			"addw %bx, (%ecx, %eax, 4)\n"
			"addw %bx, 0x45671245(%ecx, %ebx, 2)\n"

			"addl %ebx, %ecx\n"
			"addl %ebx, 0x12345678\n"
			"addl %ebx, (%ecx)\n"
			"addl %ebx, (%ecx, %eax, 4)\n"
			"addl %ebx, 0x45671245(%ecx, %ebx, 2)\n"

			"addb 0x12345678, %bh\n"

			"addw 0x34567890, %bx\n"
			"addl 0x34567890, %ebx\n"
			"addl 0x34567890(%ebx), %ebx\n"

			"addb $0x12, %al\n"
			"addw $0x1234, %ax\n"
			"addl $0x1234, %eax\n"

			"push %es\n"
			"pop %es\n"

			"adcl 0x1234, %ebx\n"
			"push %ss\n"
			"pop %ss\n"

			"inc %eax\n"
			"inc %ebx\n"
			"inc %ecx\n"
			"inc %edx\n"
			"inc %esi\n"
			"inc %edi\n"
			"inc %ebp\n"
			"inc %esp\n"

			"pushw %ax\n"
			"pushl %ebx\n"
			"pushl %ecx\n"
			"pushl %edx\n"
			"pushl %esi\n"
			"pushl %ebp\n"
			"pushl %esp\n"

			"subb $0x13, 0x12346789\n"
			"andb $0x13, 0x12345678(%ebx, %eax, 4)\n"

			"subw $0x13, 0x12346789\n"
			"andw $0x13, 0x12345678(%ebx, %eax, 4)\n"

			"subl $0x13, 0x12346789\n"
			"andl $0x13, 0x12345678(%ebx, %eax, 4)\n"

			"addr16 subw $0x13, 0x1234\n"
			"addr16 andw $0x13, 0x1234(%bx)\n"

			"sbbb   $0x34,0x78563467(%ecx,%ecx,4)\n"

			"pusha\n"
			"popa\n"

			"1:\n"
			"jz 1b\n"
		);
}

int main()
{
	uint8_t * ptr = (uint8_t*)func;
	uint8_t * prev_ptr = ptr;
	int i = 0, j = 0;
	do {
		printf("%d: ", i++);
		ptr = next_inst(ptr);
	} while (ptr != NULL);
	return 0;
}

// vim:ts=4:sw=4

