/* 
 * procmaps.c
 * by WN @ Apr. 19, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <host/procmaps.h>
#include <host/exception.h>
#include <host/mm.h>

#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define READ_STEP	(512)

char *
proc_maps_load(pid_t pid)
{
	assert(pid > 0);
	char maps_fn[32];
	snprintf(maps_fn, 32, "/proc/%d/maps", pid);
	int fd = open(maps_fn, O_RDONLY);
	if (fd < 0)
		THROW_FATAL(EXP_PROC_MAPS, "open %s failed: %s\n",
				maps_fn, strerror(errno));

	int buf_sz = READ_STEP;
	char * buffer = malloc(buf_sz);
	assert(buffer != NULL);

	char * ptr = buffer;
	int rd_sz;
	while ((rd_sz = read(fd, ptr, READ_STEP)) != 0) {
		if (rd_sz < 0) {
			close(fd);
			ERROR(PROCMAPS, "read %s failed: %s\n", maps_fn, strerror(errno));
			return NULL;
		}

		buf_sz += READ_STEP;
		buffer = realloc(buffer, buf_sz);
		assert(buffer != NULL);
		ptr = buffer + buf_sz - READ_STEP * 2 + rd_sz;

		rd_sz = read(fd, ptr, READ_STEP);
	}

	close(fd);
	return buffer;
}

void
proc_maps_free(char * buf)
{
	xfree(buf);
}


static char *
readlink_malloc (const char *filename)
{
	int size = 100;
	char *buffer = NULL;

	while (1) {
		buffer = (char *) xrealloc (buffer, size);
		assert(buffer != NULL);
		memset(buffer, '\0', size);
		int nchars = readlink (filename, buffer, size);
		if (nchars < 0) {
			xfree(buffer);
			return NULL;
		}
		if (nchars < size)
			return buffer;
		size *= 2;
	}
}

/* 
 * return value: start address of the next line.
 * if this is the last line, return NULL.
 */
static char *
read_procmem_line(char * line, struct proc_mem_region * region, char ** p_fn)
{
	int err;
	int l = 0;
	char str_prot[5];
	err = sscanf(line, "%lx-%lx %4s %x %*2x:%*2x %*d %n",
			(long unsigned int *)&region->start,
			(long unsigned int *)&region->end,
			str_prot,
			&region->offset,
			&l);

	/* set the prot */
	region->prot = 0;
	if (str_prot[0] == 'r')
		region->prot |= PROT_READ;
	if (str_prot[1] == 'w')
		region->prot |= PROT_WRITE;
	if (str_prot[2] == 'x')
		region->prot |= PROT_EXEC;
	/* str_prot[3] is 's' or 'p', we don't care 's'. when replay,
	 * all pages are 'p'. */

	/* find the mapped file name */
	*p_fn = line + l;
	if ((*(*p_fn - 1) == '\n') || (*(*p_fn - 1) == '\0'))
		*p_fn -= 1;

	/* I don't know why err == 4, but err is 4. */
	if (err != 4) {
		assert(err == 0);
		/* we meet the end of the file */
		return NULL;
	}

	/* if err != 0, then *p_fn must contain '\n', or else, it has beed set
	 * to '\0'. */
	/* eat the last '\n' in file name */
	char * eol = strchr(*p_fn, '\n');
	if (eol == NULL) {
		eol = strchr(*p_fn, '\0');
	}

	if ((eol == NULL) || ((*eol != '\0') && (*eol != '\n')))
		THROW_FATAL(EXP_PROC_MAPS, "proc maps format error: %s\n",
				line);
	*eol = '\0';
	return eol + 1;
}

void
proc_maps_find(struct proc_mem_region * preg,
		const char * file_name, char * proc_data)
{
	assert(proc_data != NULL);
	assert(file_name != NULL);
	assert(preg != NULL);

	char * full_name = (char*)file_name;
	if (file_name[0] != '[') {
		/* get the full name of the file */
		int fd = open(file_name, O_RDONLY);
		if (fd < 0)
			THROW_FATAL(EXP_PROC_MAPS, "open file %s failed: %s\n",
					file_name, strerror(errno));

		char proc_fd_name[64];
		snprintf(proc_fd_name, 64, "/proc/self/fd/%d", fd);
		full_name = readlink_malloc(proc_fd_name);
		assert(full_name != NULL);
		close(fd);
	}
	TRACE(PROCMAPS, "full name of target file is %s\n", full_name);

	define_exp(exp);
	TRY(exp) {
		char * fn;
		char * line = proc_data;
		struct proc_mem_region reg;
		bool_t found = FALSE;

		while (line != NULL) {
			line = read_procmem_line(line, &reg, &fn);
			if (strcmp(fn, full_name) == 0) {
				*preg = reg;
				found = TRUE;
				TRACE(PROCMAPS, "found %s at 0x%x--0x%x\n",
						full_name, reg.start, reg.end);
				break;
			}
		}
		if (!found)
			THROW_FATAL(EXP_PROC_MAPS, "unable to find mapping of %s\n",
					full_name);
	} FINALLY {
		if (full_name != file_name)
			xfree(full_name);
	} CATCH(exp) {
		RETHROW(exp);
	}
}

// vim:ts=4:sw=4

