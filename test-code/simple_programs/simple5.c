#include <stdlib.h>
#include <stdio.h>
int main(int argc, char * argv[])
{
	printf("argc=%d\n", argc);
	int i;
	for (i = 0; i < argc; i++)
		printf("argv[%d]=%s\n", i, argv[i]);

	while(1)
		sleep(1);
	return 0;
}

