#include <stdio.h>

int main()
{
	int i = 0;
	double a = 2.3, s = 123.45;
	while (i < 2048576) {
		printf("s=%lf\n", s += a);
		fflush(stdout);
		i++;
	}
	return 0;
}
