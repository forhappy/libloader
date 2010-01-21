#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


extern uint8_t *
next_inst(uint8_t * stream);

static void func(void)
{
	asm volatile (
		"addb $1, %al\n"
		"addw $1, %ax\n"
		"add $1, %eax\n"
		
		);
}

int main()
{
	uint8_t * ptr = (uint8_t*)func;
	do {
		ptr = next_inst(ptr);
	} while (ptr != NULL);
	return 0;
}

// vim:ts=4:sw=4

