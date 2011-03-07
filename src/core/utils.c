/* 
 * utils.c
 * by WN @ Jun. 05, 2009
 */

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "defs.h"
#include "debug.h"
#include "exception.h"
#include "utils.h"

void *
load_file(const char * file)
{
	int err;
	struct stat _s;
	assert(file != NULL);
	TRACE(SYSTEM, "Load file %s\n", file);

	err = stat(file, &_s);
	if (err != 0)
		THROW(EXCEPTION_FATAL, strerror(errno));
	if (!S_ISREG(_s.st_mode))
		THROW(EXCEPTION_FATAL, "%s is not a regular file", file);
	TRACE(SYSTEM, "file length: %d\n", _s.st_size);


	FILE * image_fp = fopen(file, "rb");
	if (image_fp == NULL)
		THROW(EXCEPTION_FATAL, "open file %s failed: %s",
				file, strerror(errno));

	void * image = NULL;
	image = malloc(_s.st_size);
	assert(image != NULL);

	err = fread(image, _s.st_size, 1, image_fp);
	if (err != 1) {
		free(image);
		fclose(image_fp);
		THROW(EXCEPTION_FATAL, "read file %s failed: %s",
				file, strerror(errno));
	}

	fclose(image_fp);

	return image;
}

#ifdef TEST_UTILS
int main(int argc, char * argv[])
{
	uint8_t * b;
	b = load_file(argv[0]);
	printf("%c%c%c\n", b[1], b[2], b[3]);
	free(b);
	return 0;
}
#endif

