/* 
 * test_debug.c
 * by WN @ Nov. 08, 2009
 */

#include <stdlib.h>
#include <common/debug.h>
#include <signal.h>

int main()
{
	dbg_init(NULL);
#ifdef SNITCHASER_DEBUG
	/* test use debug output directly */
	dbg_output(DBG_LV_TRACE, SYSTEM, __FILE__, __FUNCTION__, __LINE__,
			"system trace message with argument %d\n", 1000);
	dbg_output(DBG_LV_DEBUG, SYSTEM, __FILE__, __FUNCTION__, __LINE__,
			"system debug message with argument %d\n", 1000);
	dbg_output(DBG_LV_VERBOSE, SYSTEM, __FILE__, __FUNCTION__, __LINE__,
			"system verbose message with argument %d\n", 1000);
	dbg_output(DBG_LV_WARNING, SYSTEM, __FILE__, __FUNCTION__, __LINE__,
			"system warning message with argument %d\n", 1000);
	dbg_output(DBG_LV_ERROR, SYSTEM, __FILE__, __FUNCTION__, __LINE__,
			"system error message with argument %d\n", 1000);
	dbg_output(DBG_LV_FATAL, SYSTEM, __FILE__, __FUNCTION__, __LINE__,
			"system fatal message with argument %d\n", 1000);
	dbg_output(DBG_LV_FORCE, SYSTEM, __FILE__, __FUNCTION__, __LINE__,
			"system force message with argument %d\n", 1000);
	dbg_output(DBG_LV_SILENT, SYSTEM, __FILE__, __FUNCTION__, __LINE__,
			"system silent message with argument %d\n", 1000);

	dbg_output(DBG_LV_TRACE, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"memory trace message with argument %d\n", 1000);
	dbg_output(DBG_LV_DEBUG, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"memory debug message with argument %d\n", 1000);
	dbg_output(DBG_LV_VERBOSE, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"memory verbose message with argument %d\n", 1000);
	dbg_output(DBG_LV_WARNING, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"memory warning message with argument %d\n", 1000);
	dbg_output(DBG_LV_ERROR, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"memory error message with argument %d\n", 1000);
	dbg_output(DBG_LV_FATAL, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"memory fatal message with argument %d\n", 1000);
	dbg_output(DBG_LV_FORCE, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"memory force message with argument %d\n", 1000);
	dbg_output(DBG_LV_SILENT, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"memory silent message with argument %d\n", 1000);

	dbg_output(DBG_LV_DEBUG, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"@q test memory quiet message with argument %d\n", 1000);
	dbg_output(DBG_LV_FORCE, MEMORY, __FILE__, __FUNCTION__, __LINE__,
			"@q test memory force quiet message with argument %d\n", 1000);
#else
	/* test use debug output directly */
	dbg_output(DBG_LV_TRACE, "system trace message with argument %d\n", 1000);
	dbg_output(DBG_LV_DEBUG, "system debug message with argument %d\n", 1000);
	dbg_output(DBG_LV_VERBOSE, "system verbose message with argument %d\n", 1000);
	dbg_output(DBG_LV_WARNING, "system warning message with argument %d\n", 1000);
	dbg_output(DBG_LV_ERROR, "system error message with argument %d\n", 1000);
	dbg_output(DBG_LV_FATAL, "system fatal message with argument %d\n", 1000);
	dbg_output(DBG_LV_FORCE, "system force message with argument %d\n", 1000);
	dbg_output(DBG_LV_SILENT, "system silent message with argument %d\n", 1000);

	dbg_output(DBG_LV_TRACE, "memory trace message with argument %d\n", 1000);
	dbg_output(DBG_LV_DEBUG, "memory debug message with argument %d\n", 1000);
	dbg_output(DBG_LV_VERBOSE, "memory verbose message with argument %d\n", 1000);
	dbg_output(DBG_LV_WARNING, "memory warning message with argument %d\n", 1000);
	dbg_output(DBG_LV_ERROR, "memory error message with argument %d\n", 1000);
	dbg_output(DBG_LV_FATAL, "memory fatal message with argument %d\n", 1000);
	dbg_output(DBG_LV_FORCE, "memory force message with argument %d\n", 1000);
	dbg_output(DBG_LV_SILENT, "memory silent message with argument %d\n", 1000);
	dbg_output(DBG_LV_DEBUG, "@q test memory quiet message with argument %d\n", 1000);
	dbg_output(DBG_LV_FORCE, "@q test memory force quiet message with argument %d\n", 1000);
#endif

	/* Test the macros */
	SILENT(SYSTEM, "system silent message with arg %d\n", 10);
	TRACE(SYSTEM, "system debug message with arg %d\n", 10);
	DEBUG(SYSTEM, "system debug message with arg %d\n", 10);
	VERBOSE(SYSTEM, "system verbose message with arg %d\n", 10);
	WARNING(SYSTEM, "system warning message with arg %d\n", 10);
	ERROR(SYSTEM, "system error message with arg %d\n", 10);

	SILENT(MEMORY, "memory silent message with arg %d\n", 10);
	TRACE(MEMORY, "memory debug message with arg %d\n", 10);
	DEBUG(MEMORY, "memory debug message with arg %d\n", 10);
	VERBOSE(MEMORY, "memory verbose message with arg %d\n", 10);
	WARNING(MEMORY, "memory warning message with arg %d\n", 10);
	ERROR(MEMORY, "memory error message with arg %d\n", 10);

	/* Test memory leak detection */
	void * ptr = malloc(1024);
	free(ptr);
	raise(SIGUSR1);

	FATAL(SYSTEM, "system fatal message with arg %d\n", 10);
	return 0;
}

