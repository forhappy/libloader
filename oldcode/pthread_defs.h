/* 
 * code copy from libc 2.7.
 */
#ifndef PTHREAD_DEFS_H
#define PTHREAD_DEFS_H


typedef struct list_head
{
  struct list_head *next;
  struct list_head *prev;
} pt_list_t;


/* Get typed element from list at a given position.  */
#define pt_list_entry(ptr, type, member) \
  ((type *) ((char *) (ptr) - (unsigned long) (&((type *) 0)->member)))

/* Iterate forward over the elements of the list.  */
#define pt_list_for_each(pos, head) \
  for (pos = (head)->next; pos != (head); pos = pos->next)


/* Iterate forward over the elements of the list.  */
#define pt_list_for_each_prev(pos, head) \
  for (pos = (head)->prev; pos != (head); pos = pos->prev)


/* Iterate backwards over the elements list.  The list elements can be
   removed from the list while doing this.  */
#define pt_list_for_each_prev_safe(pos, p, head) \
  for (pos = (head)->prev, p = pos->prev; \
       pos != (head); \
       pos = p, p = pos->prev)

#endif

