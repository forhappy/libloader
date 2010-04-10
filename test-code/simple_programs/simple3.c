#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

int main()
{
	int err;
	struct timeval tv;
	err = gettimeofday(&tv, NULL);
	assert(err == 0);

	while (1) {
		struct timeval tv2;
		err = gettimeofday(&tv2, NULL);
		assert(err == 0);
		
		if (tv2.tv_sec - tv.tv_sec > 10)
			break;
	}

	return 0;
}

// vim:ts=4:sw=4:cino=l1,\:0

