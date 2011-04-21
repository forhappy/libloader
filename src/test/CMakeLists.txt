ADD_EXECUTABLE(test-1 test-1.c)
ADD_EXECUTABLE(test-2 test-2.c)
SET_PROPERTY(TARGET test-1 test-2
	APPEND
	PROPERTY LINK_FLAGS
	"-Wl,-dynamic-linker,${PROJECT_BINARY_DIR}/src/core/loader/libloader.so"
	)

