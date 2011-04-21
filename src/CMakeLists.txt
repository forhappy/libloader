INCLUDE_DIRECTORIES(
	${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/src/include
	${PROJECT_SOURCE_DIR}/core/
	)

SET(LOADER_COMPILE_FLAGS "-fvisibility=hidden -fPIC -fno-builtin-printf -fno-builtin-vsnprintf ${KERNEL_CFLAGS}")
SET(LOADER_LINK_FLAGS "-nostartfiles -nodefaultlibs -nostdlib")

#ADD_SUBDIRECTORY(common)
#ADD_SUBDIRECTORY(third-party)
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(test)

