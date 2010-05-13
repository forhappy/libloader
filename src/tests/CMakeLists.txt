SET(tests
	test_syscall
	test_linker
	test_xlinker
	test_ylinker
	)

build_tests("${tests}" "")

ADD_EXECUTABLE(interp_as_normal_so interp_as_normal_so.c)
TARGET_LINK_LIBRARIES(interp_as_normal_so interp)

IF (BUILD_TESTS)
SET_PROPERTY(TARGET test_linker
	APPEND
	PROPERTY LINK_FLAGS "-Wl,-dynamic-linker,${CMAKE_BINARY_DIR}/src/interp/libinterp.so"
	)
SET_PROPERTY(TARGET test_xlinker
	APPEND
	PROPERTY LINK_FLAGS "-Wl,-dynamic-linker,${CMAKE_BINARY_DIR}/src/interp/libxinterp.so"
	)
SET_PROPERTY(TARGET test_ylinker
	APPEND
	PROPERTY LINK_FLAGS "-Wl,-dynamic-linker,${CMAKE_BINARY_DIR}/src/interp/libinterp.so"
	)
ENDIF(BUILD_TESTS)

