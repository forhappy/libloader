INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	)
SET(SRC
	elf.c
	tls.c
	utils.c
	scan_opcode.c
	x86_opcode.h
	x86_opcode.c.h
	)

ADD_LIBRARY(platform STATIC ${SRC})

set_interp_properties(platform)

ADD_DEPENDENCIES(platform x86_tbl_parser)

ADD_CUSTOM_COMMAND(OUTPUT x86_opcode.c.h
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../utils/x86_tbl_parser < ${CMAKE_CURRENT_SOURCE_DIR}/../utils/x86_instructions.tbl > x86_opcode.c.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

ADD_SUBDIRECTORY(printf)

TARGET_LINK_LIBRARIES(platform printf)
TARGET_LINK_LIBRARIES(platform interp_common)

# vim:ts=4:sw=4

