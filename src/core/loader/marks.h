/* 
 * marks.h
 * by WN @ Nov. 23, 2010
 * marks in log file and socket transmission
 */

#ifndef INTERP_MARKS_H
#define INTERP_MARKS_H

#define RDTSC_MARK		(0x000000f1)
#define SYSCALL_MARK	(0x000000f2)
#define NO_SIGNAL_MARK	(0x000000f3)
#define SIGNAL_MARK		(0x000000f5)
#define SIGNAL_TERMINATE_MARK	(0x000000f6)
#define SIGNAL_USER_HANDLER_MARK	(0x000000f7)
#define SIGNAL_RETURN_MARK	(0x000000f8)


#endif

// vim:ts=4:sw=4

