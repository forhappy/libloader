INCLUDE_DIRECTORIES(
	${PROJECT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/src/core/
	${PROJECT_SOURCE_DIR}/src/include
	${PROJECT_SOURCE_DIR}/src/core/
	)

SET(SOURCE
	debug.c
	fdbnprintf.c
	startup_stack.c
	env_conf.c
	snitchaser_main.c
	elf.c
	syscall.c
	wrapper.c
	processor.c
	proc.c
	interp_main.c
	tls.c
	mm.c
	mutex.c
	bigbuffer.c
	signal.c
	snitchaser_tpd.c
	user_entry.c
	arch_user_entry.c
	checkpoint.c
	arch_checkpoint.c
	logger.c
	compression.c
	)

SET(ASMSOURCE
	snitchaser_asm_offset.h
	#startup.S
	user_entry.S
	)

SET(CAN_USE_ASSEMBLER TRUE)
ENABLE_LANGUAGE(ASM-S)

ADD_LIBRARY(loader SHARED ${SOURCE})
ADD_LIBRARY(asm STATIC ${ASMSOURCE})

SET_PROPERTY(TARGET loader 
	PROPERTY COMPILE_FLAGS "${LOADER_COMPILE_FLAGS}")
SET_PROPERTY(TARGET loader
	PROPERTY LINK_FLAGS "${LOADER_LINK_FLAGS}")

SET_PROPERTY(TARGET asm
	APPEND
	PROPERTY COMPILE_FLAGS "-D__ASSEMBLY__ "
	)
ADD_DEFINITIONS(-DINTERP_START_ENTRY)
TARGET_LINK_LIBRARIES(loader xprintf dummy asm log_syscall)

ADD_EXECUTABLE(snitchaser_tls_calculator snitchaser_tls_calculator.c)
SET_PROPERTY(TARGET snitchaser_tls_calculator 
	PROPERTY COMPILE_FLAGS "${LOADER_COMPILE_FLAGS}")

ADD_CUSTOM_COMMAND(OUTPUT snitchaser_asm_offset.h
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/snitchaser_tls_calculator > snitchaser_asm_offset.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS snitchaser_tls_calculator)

ADD_CUSTOM_TARGET(asmoffset
	COMMAND echo "snitchaser_asm_offset.h should has been built"
	DEPENDS snitchaser_asm_offset.h)

ADD_DEPENDENCIES(asm asmoffset)
ADD_DEPENDENCIES(loader asmoffset)


