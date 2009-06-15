
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

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
	assert_throw(fp != NULL, "open map file failed");
	return fp;
}

static void
iterate_lines(pid_t pid,
		bool_t (*checker)(uintptr_t start, uintptr_t end, char * name))
{
	FILE * fp;
	fp = open_proc_file(pid);
	char * pline = NULL;
	size_t lline = 0;
	while (getline(&pline, &lline, fp) != -1) {
		unsigned long start, end;
		int l;
		
		/* remove '\n' */
		if ((pline[strlen(pline) - 1]) == '\n')
			pline[strlen(pline) - 1] = '\0';

		sscanf(pline, "%lx-%lx %*4[-rwxp] %*x %*2d:%*2d %*d %n",
				&start, &end, &l);
		assert_throw(errno == 0, "sscanf failed: string is \"%s\"", pline);
		if (checker(start, end, pline + l)) {
			free(pline);
			pline = NULL;
			fclose(fp);
			return;
		}

		if (pline != NULL) {
			free(pline);
			pline = NULL;
		}
	}
	fclose(fp);
}

static char *
proc_get_filename(pid_t pid, uintptr_t addr,
		char * buffer, int len)
{
	buffer[0] = '\0';
	bool_t checker(uintptr_t start, uintptr_t end, char * name) {
		if ((start <= addr) && (end > addr)) {
			/* hit! */
			strncpy(buffer, name, len > 256 ? 256 : len);
			return TRUE;
		}
		return FALSE;
	}

	iterate_lines(pid, checker);

	if (buffer[0] == '\0')
		return NULL;
	return buffer;
}

void
proc_fill_entry(struct proc_entry * entry,
		pid_t pid)
{
	/* first, fix the filename */
	if (entry->bits & PE_FILE) {
		char fullname[256];
		if (entry->filename[0] != '[') {
			int fd;
			fd = open(entry->filename, O_RDONLY);
			assert_throw(fd >= 0, "Open file failed");
			void * addr = NULL;
			addr = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
			close(fd);
			assert_throw(addr != NULL, "file is not mappable");

			/* get the real filename */
			const char * perr;
			perr = proc_get_filename(getpid(), (uintptr_t)addr, fullname, 256);
			munmap(addr, 4096);
			assert_throw(perr != NULL, "can't get the full name of the file");
			strncpy(&(entry->filename[0]), fullname, 256);
		}
	}

	bool_t checker(uintptr_t start, uintptr_t end, char * name) {
		bool_t match = TRUE;
		if (entry->bits & PE_START) {
			if (start != entry->start)
				match = FALSE;
		}

		if (entry->bits & PE_END) {
			if (end != entry->end)
				match = FALSE;
		}

		if (entry->bits & PE_ADDR) {
			uint32_t addr = entry->addr;
			if ((start > addr) || (end <= addr))
				match = FALSE;
		}

		if (entry->bits & PE_FILE) {
			if (strncmp(name, entry->filename,
						strlen(entry->filename)) != 0)
				match = FALSE;
		}

		if (match) {
			entry->start = start;
			entry->end = end;
			strncpy(entry->filename, name, 256);
			entry->bits = 0xffffffffUL;
			return TRUE;
		}
		return FALSE;
	}

	iterate_lines(pid, checker);
	assert_throw(entry->bits == 0xfffffffful,
			"cannot find a matche entry in proc file");
}

const char *
proc_get_file(pid_t pid, uintptr_t addr,
		char * buffer, int len)
{
	struct proc_entry entry;
	entry.addr = addr;
	entry.bits = PE_ADDR;
	proc_fill_entry(&entry, pid);
	strncpy(buffer, entry.filename, len > 256 ? 256 : len);
	return buffer;
}

void
proc_get_range(pid_t pid, const char * filename,
		uintptr_t * pstart, uintptr_t * pend)
{
	assert(filename != NULL);
	assert(pstart != NULL);
	assert(pend != NULL);

	struct proc_entry entry;
	strncpy(entry.filename, filename, 256);
	entry.bits = PE_FILE;
	proc_fill_entry(&entry, pid);
	*pstart = entry.start;
	*pend = entry.end;
	return;
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

// vim:ts=4:sw=4
