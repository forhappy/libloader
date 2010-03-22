
#include <xasm/syscall.h>

int main()
{
	INTERNAL_SYSCALL_int80(write, 3, 1, "1234\n", 5);
	return 0;
}

// vim:ts=4:sw=4

