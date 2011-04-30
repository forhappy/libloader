#ifndef _LIST_PATCH_H
#define _LIST_PATCH_H

/* linux kernel use prefetch which generates DT_TEXTREL. this patch
 * redefine list_for_each family macros. */

#include <linux/list.h>

#ifdef list_for_each_entry
# undef list_for_each_entry
#endif
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))


#ifdef list_for_each_entry_reverse
# undef list_for_each_entry_reverse
#endif
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))

#ifdef list_for_each_entry_safe
# undef list_for_each_entry_safe
#endif
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

#endif

