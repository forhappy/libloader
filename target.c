#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
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

