#include <common/debug.h>
#include <common/exception.h>
#include <stdio.h>
#include <stdlib.h>

static void
test_func()
{
	struct exception_t exp;
	TRY(exp) {
		VERBOSE(SYSTEM, "begin try block 1\n");
		struct exception_t exp_i;
		TRY(exp_i) {
			VERBOSE(SYSTEM, "begin try block 1-1\n");
			THROW(EXP_RESOURCE_NOT_FOUND,
					"this exception is raised in try block %d-%d", 1, 1);
			FATAL(SYSTEM, "we shouldn't be here\n");
		} FINALLY {
			VERBOSE(SYSTEM, "in finally block 1-1\n");
		} CATCH(exp_i) {
			VERBOSE(SYSTEM, "in catch block 1-1\n");
			switch (exp_i.type) {
				case EXP_RESOURCE_NOT_FOUND:
					VERBOSE(SYSTEM, "caught an exception: %s\n",
							exp_i.msg);
					break;
				default:
					RETHROW(exp_i);
			}
		}

		VERBOSE(SYSTEM, "try block 1-1 end\n");

		struct exception_t exp_j;
		TRY(exp_j) {
			VERBOSE(SYSTEM, "begin try block 1-2\n");
			struct exception_t exp_k;
			TRY(exp_k) {
				VERBOSE(SYSTEM, "begin try block 1-2-1\n");
				THROW(EXP_UNCATCHABLE, "throw an uncatchable exception");
				FATAL(SYSTEM, "we shouldn't be here\n");
			} FINALLY {
				VERBOSE(SYSTEM, "in finally block 1-2-1\n");
			}
			CATCH(exp_k) {
				VERBOSE(SYSTEM, "in catch block 1-2-1\n");
				RETHROW(exp_k);
			}
		} FINALLY {
			VERBOSE(SYSTEM, "in finally block 1-2\n");
		}
		CATCH(exp_j) {
			VERBOSE(SYSTEM, "in catch block 1-2\n");
			RETHROW(exp_j);
		}

		FATAL(SYSTEM, "We shouldn't be here\n");
	} FINALLY {
		VERBOSE(SYSTEM, "in finally block 1\n");
	} CATCH(exp) {
		VERBOSE(SYSTEM, "in catch block 1\n");
		switch (exp.type) {
			default:
				RETHROW(exp);
		}
	}
}

int main()
{
	dbg_init(NULL);
	FORCE(SYSTEM, "-------------------------\n");
	struct exception_t exp;
	TRY(exp) {
		VERBOSE(SYSTEM, "in try block\n");
	} FINALLY {
		VERBOSE(SYSTEM, "in finally block\n");
	} CATCH(exp) {
		VERBOSE(SYSTEM, "in catch block\n");
	}

	FORCE(SYSTEM, "-------------------------\n");
	TRY(exp) {
		VERBOSE(SYSTEM, "in try block, before throw\n");
		THROW(EXP_RESOURCE_NOT_FOUND,
				"throw exception, %d", 100);
		FATAL(SYSTEM, "we shouldn't be here\n");
	} FINALLY {
		VERBOSE(SYSTEM, "in finally block\n");
	} CATCH(exp) {
		VERBOSE(SYSTEM, "in catch block\n");
		switch (exp.type) {
			case EXP_RESOURCE_NOT_FOUND:
				VERBOSE(SYSTEM, "catch resource not found\n");
				print_exception(&exp);
				break;
			default:
				RETHROW(exp);
		}
	}
	FORCE(SYSTEM, "------------begin multilevel exception----------\n");
	test_func();

	return 0;
}
