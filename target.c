#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
	printf("I am target\n");
	printf("argc=%d, argv[0]=%s\n",
			argc, argv[0]);
	while(1)
		sleep(1);
	return 0;
}

