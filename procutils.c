
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
	char fn[64];
	memset(fn, '\0', sizeof(fn));
	snprintf(fn, 64, "/proc/%d/maps", pid);
	fp = fopen(fn, "r");
	assert_throw(fp != NULL, "open map file failed");
	return fp;
}

static void
iterate_lines(pid_t pid,
		bool_t (*checker)(uintptr_t start, uintptr_t end,
			uint32_t prot, uint32_t offset, char * name))
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

		char prots[5];
		uint32_t offset;
		uint32_t prot = 0;
		
		sscanf(pline, "%lx-%lx %4[-rwxp] %x %*2x:%*2x %*d %n",
				&start, &end, prots, &offset, &l);
		assert_throw(errno == 0, "sscanf failed: string is \"%s\"", pline);

		if (prots[0] == 'r')
			prot |= PROT_READ;
		if (prots[1] == 'w')
			prot |= PROT_WRITE;
		if (prots[2] == 'x')
			prot |= PROT_EXEC;
		/* prots[3] is 's' or 'p', but we don't care 's', make all
		 * pages 'p'. */

		/* if checker returns TRUE, stop iterate */
		if (checker(start, end, prot, offset, pline + l)) {
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
	bool_t checker(uintptr_t start, uintptr_t end,
			uint32_t prot, uint32_t offset, char * name)
	{
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

/* 
 * this function retrive the 1st (lowest) entry satisfy
 * entry's equirements
 */
int
proc_fill_entry(struct proc_entry * entry,
		pid_t pid)
{
	/* first, fix the filename */
	if (entry->bits & PE_FILE) {
		char fullname[256];
		if (entry->fn[0] != '[') {
			int fd;
			fd = open(entry->fn, O_RDONLY);
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
			strncpy(&(entry->fn[0]), fullname, 256);
		}
	}

	bool_t checker(uintptr_t start, uintptr_t end,
			uint32_t prot, uint32_t offset, char * name)
	{
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
			if (strncmp(name, entry->fn,
						strlen(entry->fn)) != 0)
				match = FALSE;
		}

		if (match) {
			entry->start = start;
			entry->end = end;
			strncpy(entry->fn, name, 256);
			entry->bits = 0xffffffffUL;
			entry->prot = prot;
			entry->offset = offset;
			return TRUE;
		}
		return FALSE;
	}

	iterate_lines(pid, checker);
	if (entry->bits != 0xffffffffUL)
		return -1;
	return 0;
}


bool_t
proc_find_in_range(struct proc_entry * entry,
		pid_t pid, uintptr_t start, uintptr_t end)
{
	entry->bits = 0UL;
	bool_t checker(uintptr_t s, uintptr_t e,
			uint32_t prot, uint32_t offset, char * name)
	{
		if ((e <= start) || (s >= end)) {
			/* not overlap, continue iterate */
			return FALSE;
		}

		/* fill entry */
		entry->start = s;
		entry->end = e;
		strncpy(entry->fn, name, 256);
		entry->bits = 0xffffffffUL;
		entry->prot = prot;
		entry->offset = offset;
		/* stop iterate */
		return TRUE;
	}
	iterate_lines(pid, checker);
	if (entry->bits != 0xffffffffUL) {
		/* not found */
		return FALSE; 
	}
	return TRUE;
}

const char *
proc_get_file(pid_t pid, uintptr_t addr,
		char * buffer, int len)
{
	int err;
	struct proc_entry entry;
	entry.addr = addr;
	entry.bits = PE_ADDR;
	err = proc_fill_entry(&entry, pid);
	CTHROW(err == 0, "no file mapped at 0x%x--0x%x", addr, addr + len);
	strncpy(buffer, entry.fn, len > 256 ? 256 : len);
	return buffer;
}

void
proc_get_range(pid_t pid, const char * fn,
		uintptr_t * pstart, uintptr_t * pend)
{
	int err;

	assert(fn != NULL);
	assert(pstart != NULL);
	assert(pend != NULL);

	struct proc_entry entry;
	strncpy(entry.fn, fn, 256);
	entry.bits = PE_FILE;
	err = proc_fill_entry(&entry, pid);
	CTHROW(err == 0, "file %s not found\n", fn);
	*pstart = entry.start;
	*pend = entry.end;
	return;
}

#ifdef TEST_PROCUTILS
int main()
{
	char fn[128];
	DEBUG_INIT(NULL);
	printf("%s\n", proc_get_file(getpid(), (unsigned long)(&main), fn, 128));

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
