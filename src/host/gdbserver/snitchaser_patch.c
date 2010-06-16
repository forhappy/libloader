/* 
 * snitchaser_patch.c
 * by WN @ Jun. 16, 2010
 */

#include <host/gdbserver/snitchaser_patch.h>

pid_t target_original_pid;
pid_t target_original_tid;
int target_tnr;
void * target_stack_base;

