/* 
 * dict.c
 * by WN @ Mar. 10, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/bithacks.h>
#include <asm/string.h>
#include <interp/mm.h>
#include <interp/dict.h>

struct dict_t *
create_dict(int max_slots)
{
	assert(max_slots > 0);
	max_slots = pow2roundup(max_slots);
	int real_sz = DICT_REAL_SZ(max_slots);
	struct dict_t * dict = alloc_cont_space(real_sz);
	assert(dict != NULL);
	dict->mask = max_slots - 1;
	dict->active = 0;
	memset(dict->table, '\0', sizeof(struct dict_entry_t) * max_slots);
	TRACE(DICT, "create dict %p, max_slots is %d\n", dict, max_slots);
	return dict;
}

void
destroy_dict(struct dict_t ** pdict)
{
	assert(pdict != NULL);
	struct dict_t * dict = *pdict;
	if (dict == NULL)
		return;
	TRACE(DICT, "dict %p destroied\n", dict);
	free_cont_space(dict);
	*pdict = NULL;
}

static struct dict_entry_t *
get_entry(struct dict_t * dict, uintptr_t key)
{
	int size = dict->mask + 1;
	int nr_checked = 0;
	uintptr_t hash = key;
	struct dict_entry_t * ep0 = dict->table;
	struct dict_entry_t * ep;
	int i = hash & dict->mask;
	TRACE(DICT, "finding entry 0x%x from dict %p\n", key, dict);
	do {
		ep = &(ep0[i & dict->mask]);
		TRACE(DICT, "n = %d\n", i & dict->mask);
		if ((ep->key == key) || (ep->key == NO_SUCH_KEY)) {
			TRACE(DICT, "found 0x%x for 0x%x at %p, value is 0x%x\n",
					ep->key, key, ep, ep->value);
			return ep;
		}
		if (ep->key == DUMMY_KEY)
			FATAL(DICT, "dict %p contains dummy key, that shouldn't happen\n",
					dict);
		i = (i << 2) + i + 1;
		nr_checked ++;
	} while (nr_checked < size);
	TRACE(DICT, "doesn't find 0x%x from dict %p\n", key, dict);
	return NULL;
}

/* dict is guarantee to have enough space */
static void
dict_insert_clean(struct dict_t * dict, uintptr_t key, uintptr_t value)
{
	struct dict_entry_t * ep = get_entry(dict, key);
	assert(ep != NULL);
	ep->key = key;
	ep->value = value;
	dict->active ++;
}

static void
copy_dict(struct dict_t * dst, struct dict_t * src)
{
	assert(dst->mask >= src->mask);
	struct dict_entry_t * ep0 = src->table;
	for (int i = 0; i < (int)(src->mask) + 1; i++) {
		struct dict_entry_t * oep = &ep0[i];
		if ((oep->key == NO_SUCH_KEY) || (oep->key == DUMMY_KEY))
			continue;
		uintptr_t key = oep->key;
		uintptr_t value = oep->value;
		dict_insert_clean(dst, key, value);
	}
	return;
}

void
dict_insert(struct dict_t **pdict, uintptr_t key, uintptr_t value)
{
	assert(pdict != NULL);
	assert(key != 0);
	TRACE(DICT, "insert (0x%x, 0x%x) into dict %p\n", key, value, *pdict);

	struct dict_t * dict = *pdict;
	if (dict == NULL) {
		dict = create_dict(DEFAULT_DICT_SLOTS);
		*pdict = dict;
	}
	if ((dict->active * 3) > (int)(2 * dict->mask) + 2) {
		/* build new dict */
		struct dict_t * new_dict = create_dict(dict->mask * 2 + 1);
		assert(new_dict != NULL);
		/* copy entries */
		copy_dict(new_dict, dict);
		/* destroy old dict */
		destroy_dict(&dict);
		dict = new_dict;
		*pdict = dict;
	}
	dict_insert_clean(dict, key, value);
	return;
}

uintptr_t
dict_get(struct dict_t * dict, uintptr_t key)
{
	if (dict == NULL)
		return 0;
	struct dict_entry_t * ep = get_entry(dict, key);
	if (ep == NULL)
		return 0;
	if (ep->key == NO_SUCH_KEY)
		return 0;
	if (ep->key == DUMMY_KEY)
		return 0;

	assert(ep->key == key);
	return ep->value;
}

// vim:ts=4:sw=4

