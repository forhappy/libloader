#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

static int global_data = 0;

int main(int argc, char * argv[])
{
//	volatile int x = 0;
//	while (x == 0);

	printf("argv[0] is \"%s\", at %p\n", argv[0], &argv[0]);
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

	while(1);

//	FILE * fp = fopen("/dev/null", "w");
	while(1) {
		int c = 'x';
//		c = getchar();
//		volatile int x = 1;
//		while(x == 1);
//		sleep(1);
		fprintf(stdout, "%d, %c, global_data=%d\n", i++, c, global_data);
		fflush(stdout);
		if (i == 17)
			break;
//		if (i == 1784705)
//			break;
	}
	return 0;
}

