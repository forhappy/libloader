
#include <stdint.h>
#include <dlfcn.h>

void __vsyscall()
{
	return;
}

int main()
{
	while(1);
	return 0;
}

