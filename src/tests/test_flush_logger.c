#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{

	printf("program runs normally\n");
	int i = 0;
	while(i < 100000000)
		i++;
	printf("end\n");
	return 0;
}

// vim:ts=4:sw=4

