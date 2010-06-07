#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{

	printf("program runs normally\n");
	int i = 0;
	int s = 0;
	while(i < 100000000)
		s += i++;
	printf("end\n");
	return s;
}

// vim:ts=4:sw=4

