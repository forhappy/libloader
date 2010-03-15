INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	)

SET(CAN_USE_ASSEMBLER TRUE)
ENABLE_LANGUAGE(ASM-S)

SET(SRC
	startup.S
	elf.c
	tls.c
	utils.c
	compiler.c
	scan_opcode.c
	x86_opcode.h
	x86_opcode.c.h
	asm_offsets.h
	)

ADD_LIBRARY(platform STATIC ${SRC})

set_interp_properties(platform)

ADD_DEPENDENCIES(platform x86_tbl_parser)
ADD_CUSTOM_COMMAND(OUTPUT x86_opcode.c.h
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../utils/x86_tbl_parser < ${CMAKE_CURRENT_SOURCE_DIR}/../utils/x86_instructions.tbl > x86_opcode.c.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
ADD_DEPENDENCIES(platform compute_tls_offset)
ADD_CUSTOM_COMMAND(OUTPUT asm_offsets.h
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/compute_tls_offset > asm_offsets.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)



ADD_SUBDIRECTORY(printf)

TARGET_LINK_LIBRARIES(platform printf)
TARGET_LINK_LIBRARIES(platform interp_common)

ADD_EXECUTABLE(compute_tls_offset compute_tls_offset.c)

# vim:ts=4:sw=4

