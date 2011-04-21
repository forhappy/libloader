#ifndef __PROC_H
#define __PROC_H
#include <defs.h>
struct proc_maps_entry_t{
	bool_t valid;
	uintptr_t start;
	uintptr_t end;
	uint32_t prot;
	uint64_t offset;
	uint32_t major_dev;
	uint32_t minor_dev;
	uint32_t inode;
	char * fn;
	size_t fn_len;
};

struct proc_mem_handler_t {
	char * map_data;
	char * map_end;
	char * curr_ptr;
};

extern void
init_map_handler(struct proc_mem_handler_t * handler,
		char * data, size_t size);

extern void
reset_map_handler(struct proc_mem_handler_t * handler);

/* if r.valid is false, this is the last line */
extern struct proc_maps_entry_t
read_maps_line(struct proc_mem_handler_t *handler);

extern struct proc_maps_entry_t
find_in_maps_by_fn(struct proc_mem_handler_t *handler,
		const char * file_name, bool_t reset);

extern struct proc_maps_entry_t
find_in_maps_by_addr(struct proc_mem_handler_t *handler,
		const void * ptr);
extern void 
read_pid_procmaps(int pid, struct proc_mem_handler_t *handler, void *ptr);

extern void 
read_self_procmaps(struct proc_mem_handler_t *handler, void *ptr);
extern void
flush_mem_to_ckpt(int ckptfd);
#endif
