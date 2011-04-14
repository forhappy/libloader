/* proc.c derived frome rebranch/commmon/procmap.c
 * by HP.Fu @ Apr.6, 2011
 * */
#include "proc.h"
#include "vsprintf.h"
#include "exception.h"
#include "debug.h"

#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02

#define PROT_READ	0x1		/* page can be read */
#define PROT_WRITE	0x2		/* page can be written */
#define PROT_EXEC	0x4		/* page can be executed */
#define PROT_SEM	0x8		/* page may be used for atomic ops */
#define PROT_NONE	0x0		/* page can not be accessed */
#define PROT_GROWSDOWN	0x01000000	/* mprotect flag: extend change to start of growsdown vma */
#define PROT_GROWSUP	0x02000000	/* mprotect flag: extend change to end of growsup vma */

#define MAP_EXECUTABLE	0x1000		/* mark it as an executable */
#define MAP_SHARED	0x01		/* Share changes */
#define MAP_PRIVATE	0x02		/* Changes are private */
#define MAP_TYPE	0x0f		/* Mask for type of mapping */
#define MAP_FIXED	0x10		/* Interpret addr exactly */
#define MAP_ANONYMOUS	0x20		/* don't use a file */
#define MAX_PROC_MAPS_FILE_SIZE ((2048)*(1024))
void
init_map_handler(struct proc_mem_handler_t * handler,
		char * data, size_t size)
{
	assert(handler != NULL);
	assert(data != NULL);
	assert(size > 0);
	handler->map_data = data;
	handler->map_end = data + size;
	handler->curr_ptr = data;
	return;
}

void
reset_map_handler(struct proc_mem_handler_t * handler)
{
	assert(handler != NULL);
	assert(handler->map_data != NULL);
	handler->curr_ptr = handler->map_data;
}

static inline char *
next_ptr(char * ptr, char * end)
{
	assert(ptr < end);
	return ptr + 1;
}

static char *
skip_spaces_and_eol(char * ptr, char * end)
{
	while ((*ptr == '\t') ||
			(*ptr == ' ') ||
			(*ptr == '\n'))
		ptr = next_ptr(ptr, end);
	return ptr;
}

/* return the last '\n' */
static char *
get_eol(char * ptr, char * end)
{
	while ((*ptr != '\n') && (*ptr != '\0'))
		ptr = next_ptr(ptr, end);
	assert((*ptr == '\n') || (*ptr == '\0'));
	return ptr;
}

static struct proc_maps_entry_t
parse_line(char * ptr, size_t len)
{
	struct proc_maps_entry_t entry;
	char str_prot[8];
	int l;
	entry.valid = TRUE;
	int err = sscanf(ptr, "%lx-%lx %4s %llx %*2x:%*2x %*d %n",
			(long unsigned int *)(&entry.start),
			(long unsigned int *)(&entry.end),
			str_prot,
			(long long unsigned int *)(&entry.offset),
			&l);
	assert(err == 4);

	entry.prot = 0;
	if (str_prot[0] == 'r')
		entry.prot |= PROT_READ;
	if (str_prot[1] == 'w')
		entry.prot |= PROT_WRITE;
	if (str_prot[2] == 'x')
		entry.prot |= PROT_EXEC;

	/* filename... */
	char * fn_ptr = ptr + l;
	while ((*fn_ptr == ' ') || (*fn_ptr == '\t'))
		fn_ptr ++;
	/* FIXME:findname should start with '/', if not, it indicates the filename
	 * is NULL,in which situation we get the proc_maps_entry_t as follows:
	 * "b78d5000-b78d7000 rw-p 00000000 00:00 0 \n"
	 * but fn_ptr doesn't point to the right place(usually the first character 
	 * of the next proc_maps_entry_t, which is absolutly wrong),we should
	 * operate fn_ptr--;*/
	if (*fn_ptr != '/' && *fn_ptr != '[') fn_ptr--;
	if (*fn_ptr == '\n') {
		entry.fn = NULL;
		entry.fn_len = 0;
	} else {
		entry.fn = fn_ptr;
		assert(ptr + len > fn_ptr);
		entry.fn_len = ptr + len - fn_ptr;
	}

	if (entry.fn!= NULL) {
		/* check file name */
		for (unsigned int i = 0; i < entry.fn_len; i++) {
			if (entry.fn[i] == '\\') {
				entry.fn[entry.fn_len] = '\0';
				FATAL(CKPT, "file %s contain special character at %d\n",
						entry.fn, i);
			}
		}
	}

	return entry;
}

/* if entry.valid is false, this is the last line */
struct proc_maps_entry_t
read_maps_line(struct proc_mem_handler_t * handler)
{
	assert(handler != NULL);
	assert(handler->map_data != NULL);
	assert(handler->map_end > handler->map_data);

	if (handler->curr_ptr >= handler->map_end) {
		struct proc_maps_entry_t entry;
		memset(&entry, '\0', sizeof(entry));
		entry.valid = FALSE;
		return entry;
	}

	char * ptr = handler->curr_ptr;
	char * end = handler->map_end;
	ptr = skip_spaces_and_eol(ptr, end);

	char * eol = get_eol(ptr, end);
	size_t len = eol - ptr;

	handler->curr_ptr = eol + 1;
	return parse_line(ptr, len);
}

struct proc_maps_entry_t
find_in_map_by_fn(struct proc_mem_handler_t * handler,
		const char * fn, bool_t reset)
{
	struct proc_maps_entry_t entry;
	assert(handler != NULL);
	assert(fn != NULL);

	size_t fn_len = strlen(fn);
	if (reset)
		reset_map_handler(handler);

	entry = read_maps_line(handler);
	while (entry.valid) {
		if (entry.fn!= NULL) {
			assert(entry.fn_len != 0);
			if (entry.fn_len == fn_len) {
				if (memcmp(entry.fn, fn, fn_len) == 0)
					return entry;
			}
		}
		entry = read_maps_line(handler);
	}

	memset(&entry, '\0', sizeof(entry));
	entry.valid = FALSE;
	return entry;
}

struct proc_maps_entry_t
find_in_map_by_addr(struct proc_mem_handler_t * handler, const void * addr)
{
	assert(handler != NULL);
	struct proc_maps_entry_t entry;
	reset_map_handler(handler);
	entry = read_maps_line(handler);
	while (entry.valid) {
		uintptr_t p = (uintptr_t)(addr);
		if ((entry.start <= p) && (entry.end > p))
			return entry;
		entry = read_maps_line(handler);
	}
	memset(&entry, '\0', sizeof(entry));
	entry.valid = FALSE;
	return entry;
}


void
read_self_procmap(struct proc_mem_handler_t * handler, void * ptr)
{
	assert(ptr != NULL);
	assert(handler != NULL);
	int fd = sys_open("/proc/self/maps", O_RDONLY, 0);
	CASSERT(fd >= 0, CKPT, "open /proc/self/maps failed: %d\n", fd);

	int err = sys_read(fd, ptr, MAX_PROC_MAPS_FILE_SIZE);
	CASSERT(err > 0, CKPT, "read /proc/self/maps returns %d\n", err);
	CASSERT(err < MAX_PROC_MAPS_FILE_SIZE,  CKPT, "/proc/self/maps too large, "
			"increase MAX_PROC_MAPS_FILE_SIZE\n");
	sys_close(fd);

	init_map_handler(handler, ptr, (size_t)(err));
}


void
read_pid_procmap(int pid, struct proc_mem_handler_t * handler, void * ptr)
{
	assert(handler != NULL);
	assert(ptr != NULL);

	char maps_name[64];
	snprintf(maps_name, 64, "/proc/%d/maps", pid);
	int fd = sys_open(maps_name, O_RDONLY, 0);
	if (fd < 0)
		THROW_FATAL(EXP_IO, "fail to open maps %s", maps_name);

	int err = sys_read(fd, ptr, MAX_PROC_MAPS_FILE_SIZE);
	if ((err < 0) || (err >= MAX_PROC_MAPS_FILE_SIZE))
		THROW_FATAL(EXP_IO, "read %s returns %d", maps_name, err);

	sys_close(fd);
	init_map_handler(handler, ptr, (size_t)err);
}

void
flush_mem_to_ckpt(int ckptfd)
{

	char fn[256];
	snprintf(fn, 256, "/proc/self/maps");
	DEBUG(SYSTEM, "map file: %s\n", fn);

	int fd = sys_open(fn, O_RDONLY, 0);

	char * buffer = sys_mmap2(NULL, 1 << 20, \
			PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	CASSERT(buffer != NULL, CKPT, "sys_mmap2 failed\n");
	int sz = sys_read(fd, buffer, 1 << 20);
	CASSERT(sz > 0, CKPT, "sz < 0\n");
	CASSERT(sz < (1 << 20), CKPT, "sz > 1MB\n");
	struct proc_mem_handler_t h;
	init_map_handler(&h, buffer, sz);
	struct proc_maps_entry_t entry;
	entry = read_maps_line(&h);

	while (entry.valid) {
		sys_write(ckptfd, (void *)entry.start,(int)(entry.end - entry.start));
		entry = read_maps_line(&h);
	}
	return;
}
