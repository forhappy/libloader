SET(tests
	test_syscall
	test_linker
	)

build_tests("${tests}" "")
IF (BUILD_TESTS)
SET_PROPERTY(TARGET test_linker
	APPEND
	PROPERTY LINK_FLAGS "-Wl,-dynamic-linker,${CMAKE_BINARY_DIR}/src/interp/libinterp.so"
	)
ENDIF(BUILD_TESTS)

