/* 
 * replayer.c
 * by WN @ Apr. 26, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <xasm/utils.h>

__attribute__((used, unused, visibility("hidden"))) void
replayer_main(void)
{
	relocate_interp();
	VERBOSE(REPLAYER, "XXXXX\n");
}

// vim:ts=4:sw=4

