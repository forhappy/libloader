
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "procutils.h"
#include "debug.h"
#include "exception.h"

static FILE *
open_proc_file(pid_t pid)
{
	FILE * fp;
	char filename[64];
	memset(filename, '\0', sizeof(filename));
	snprintf(filename, 64, "/proc/%d/maps", pid);
	fp = fopen(filename, "r");
	if (fp == NULL)
		THROW(EXCEPTION_FATAL, "open map file failed");
	return fp;
}

const char *
proc_get_file(pid_t pid, uintptr_t addr,
		char * buffer, int len)
{
	FILE * fp;
	fp = open_proc_file(pid);
	char * pline = NULL;
	size_t lline = 0;
	while (getline(&pline, &lline, fp) != -1) {
		unsigned long start, end;
		int l;
		sscanf(pline, "%lx-%lx %*4[-rwxp] %*x %*2d:%*2d %*d %n",
				&start, &end, &l);
		if ((start <= addr) && (end > addr)) {
			/* find filename */
			strncpy(buffer, pline + l, len);
			/* remove the last '\n' */
			buffer[strlen(buffer) - 1] = '\0';
			free(pline);
			fclose(fp);
			return buffer;
		}
		if (pline != NULL) {
			free(pline);
			pline = NULL;
		}
	}
	
	fclose(fp);
	return NULL;
}

void
proc_get_range(pid_t pid, const char * filename,
		uintptr_t * pstart, uintptr_t * pend)
{
	assert(filename != NULL);
	assert(pstart != NULL);
	assert(pend != NULL);
	/* first, mmap the file into myself's memory space
	 * to get the precise name of it */
	char fullname[256];
	if (filename[0] != '[') {
		int fd;
		fd = open(filename, O_RDONLY);
		if (fd < 0)
			THROW(EXCEPTION_FATAL, "Open file failed");
		void * addr = NULL;
		addr = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
		close(fd);

		if (addr == NULL)
			THROW(EXCEPTION_FATAL, "file is not mappable");

		/* second: get the real filename */
		const char * perr;
		perr = proc_get_file(getpid(), (uintptr_t)addr, fullname, 256);
		munmap(addr, 4096);
		if (perr == NULL)
			THROW(EXCEPTION_FATAL, "can't get the full name of the file");
	} else {
		strncpy(fullname, filename, 256);
	}

	/* match the filename */
	FILE * fp;
	fp = open_proc_file(pid);
	char * pline = NULL;
	size_t lline = 0;
	*pstart = *pend = 0;
	while (getline(&pline, &lline, fp) != -1) {
		unsigned long start, end;
		int l;
		sscanf(pline, "%lx-%lx %*4[-rwxp] %*x %*2d:%*2d %*d %n",
				&start, &end, &l);
		if (strncmp(pline + l, fullname, strlen(fullname)) == 0) {
			/* file name matched */
			if (*pstart == 0)
				*pstart = start;
			if (*pend < end)
				*pend = end;
		}

		if (pline != NULL) {
			free(pline);
			pline = NULL;
		}
	}


	if (pline != NULL) {
		free(pline);
		pline = NULL;
	}
	fclose(fp);
	return ;
}

#ifdef TEST_PROCUTILS
int main()
{
	char filename[128];
	DEBUG_INIT(NULL);
	printf("%s\n", proc_get_file(getpid(), (unsigned long)(&main), filename, 128));

	uintptr_t s, e;
	proc_get_range(getpid(), "./test_proc", &s, &e);
	printf("0x%x, 0x%x\n", s, e);
	proc_get_range(getpid(), "[stack]", &s, &e);
	printf("0x%x, 0x%x\n", s, e);
	do_cleanup();
	return 0;
}
#endif

