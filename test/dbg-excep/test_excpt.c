#include "defs.h"
#include "rb_debug.h"
#include "rb_exception.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
void dummy()
{
	CTHROW_FATAL(1 == 0, EXP_IO, "1 == 0 failed\n" );
}
void main()
{
	define_exp(exp);
	TRY(exp){
		//__asm__ volatile("int3");
		dummy();
	} FINALLY {
	} CATCH(exp) {
		print_exception(&exp);
		VERBOSE(SYSTEM, "rebranch terminate\n");
	}
}
