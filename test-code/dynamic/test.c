#include <stdio.h>

static int static_data[10] = {1,2,3,4,5,6,7,8,9,0};
int extern_data[10] = {11,22,33,44,55,66,77,88,99,1010};
int extern_data2[10] = {11,22,33,44,55,66,77,88,99,1010};
extern int _DYNAMIC[];

extern int my_data[];

int func(void)
{
	printf("Hahahaha!!! %p\n",
			static_data);
	printf("Hahahaha!!! %p\n",
			extern_data);
	printf("_DYNAMIC: %p\n",
			_DYNAMIC);
#if 0
	printf("Hahahaha!!! %p\n",
			extern_data2);
	printf("Hahahaha!!! %p\n",
			my_data);
	printf("Hahahaha!!! %d\n",
			static_data[5]);
	printf("Hahahaha!!! %d\n",
			extern_data[5]);
#endif
	return 0;
}
