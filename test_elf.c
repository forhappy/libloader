#include <elfutils.h>

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

int main()
{
	DEBUG_INIT(NULL);
	VERBOSE(SYSTEM, "System start!\n");
	FORCE(SYSTEM, "Force message\n");
	return 0;
}

