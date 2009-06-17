#include <stdio.h>
extern void syscall_wrapper_entrace(void);
int main()
{
	syscall_wrapper_entrace();
	return 0;
}

