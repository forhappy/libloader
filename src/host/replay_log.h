/* 
 * replay_log.h
 * by WN @ Jun. 21, 2010
 *
 * log utils for replayer
 *
 */

#ifndef REPLAY_LOG_H
#define REPLAY_LOG_H

#include <common/defs.h>

void
open_log(const char * fn);

void
close_log(void);

void
read_log_full(void * buf, size_t size);

size_t
read_log(void * buf, size_t size);

void
uncompress_log(const char * log_fn, const char * out_fn);

#endif

// vim:ts=4:sw=4

