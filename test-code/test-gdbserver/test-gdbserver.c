
#include <stdio.h>
#include <stdlib.h>

static void
func(void)
{
	printf("in func %s, line %d\n", __func__, __LINE__);
	return;
}

int main()
{
	printf("in func %s, line %d\n", __func__, __LINE__);
	printf("in func %s, line %d\n", __func__, __LINE__);
	sleep(1);
	func();
	printf("in func %s, line %d\n", __func__, __LINE__);

	return 0;
}

// vim:ts=4:sw=4

