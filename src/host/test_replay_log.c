#include <common/defs.h>
#include <common/debug.h>
#include <host/exception.h>
#include <host/replay_log.h>
#include <assert.h>

int main(int argc ATTR_UNUSED, char * argv[])
{
	char * fn = argv[1];
	assert(fn != NULL);

	open_log(fn);

	define_exp(exp);
	TRY(exp) {
		uintptr_t ptr = readahead_log_ptr();
		VERBOSE(SYSTEM, "ptr1 0x%x\n", ptr);

		ptr = read_ptr_from_log();
		VERBOSE(SYSTEM, "ptr2 0x%x\n", ptr);

		ptr = read_ptr_from_log();
		VERBOSE(SYSTEM, "ptr3 0x%x\n", ptr);

		ptr = readahead_log_ptr();
		VERBOSE(SYSTEM, "ptr4 0x%x\n", ptr);

		ptr = read_ptr_from_log();
		VERBOSE(SYSTEM, "ptr5 0x%x\n", ptr);
	} FINALLY {
		close_log();
	} CATCH(exp) {
		RETHROW(exp);
	}


	return 0;
}

// vim:ts=4:sw=4

