#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char * argv[])
{
	int res;
#if 0
	asm volatile ("int $0x80"
			: "=a" (res)
			: "a" (192),
				"b" (8192), "c" (4096), "d" (PROT_READ), "S" (MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS)
			);
	printf("%x\n", res);
#endif


	/* check for breakpoint */
	printf("addr of main = %p\n", main);
	printf("dup 8 bytes from main: ");
	for (int i = 0; i < 8; i++)
		printf("0x%x ", ((unsigned char*)main)[i]);
	printf("\n");

	printf("I am target\n");
	printf("argc=%d, argv[0]=%s\n",
			argc, argv[0]);
	int i = 1;
	while(1) {
		int c = 'x';
//		c = getchar();
		sleep(1);
		printf("%d, %c\n", i++, c);
	}
	return 0;
}

