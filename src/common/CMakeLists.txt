SET(SRC
	defs.h
	debug_components.h
	debug.c
	debug.h
	exception.h
	exception.c
	)

ADD_LIBRARY(common STATIC ${SRC})

SET(tests
	test_debug
	test_exception)
build_tests("${tests}" "common")


