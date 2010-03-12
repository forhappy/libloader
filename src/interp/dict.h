/* 
 * dict.h
 * by WN @ Mar. 10, 2010
 */

#ifndef __SNITCHASER_DICT_H
#define __SNITCHASER_DICT_H

#define NO_SUCH_KEY	(0)
#define DUMMY_KEY	(1)

#define DEFAULT_DICT_SLOTS	(100000)

struct dict_entry_t {
	uintptr_t key;
	uintptr_t value;
};

/* in fact, we never remove any key from our dict, so
 * DUMMY_KEY is useless. */
struct dict_t {	
	/* mask is size - 1, size is always power of 2 */
	uint32_t mask;
	/* active slots, if (active * 3) / ((mask + 1) * 2), we need
	 * to realloc dict */
	int active;
	struct dict_entry_t table[0];
};

#define DICT_REAL_SZ(nr)	(sizeof(struct dict_t) + sizeof(struct dict_entry_t) * nr)

extern struct dict_t *
create_dict(int max_slots);

extern void
destroy_dict(struct dict_t * dict);

extern void
dict_insert(struct dict_t ** pdict, uintptr_t key, uintptr_t value);

/* if return value is NO_SUCH_KEY or DUMMY_KEY, then the key is not in dict */
extern uintptr_t
dict_get(struct dict_t * dict, uintptr_t key);


#endif

// vim:ts=4:sw=4

