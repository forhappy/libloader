SET(SOURCE
	string.c
	vsprintf.c
	string_helpers.c
	ctype.c
	hexdump.c
	string_32.c
	memcpy_32.c
	)

SET(CAN_USE_ASSEMBLER TRUE)
ENABLE_LANGUAGE(ASM-S)

ADD_LIBRARY(xprintf STATIC ${SOURCE})
SET_PROPERTY(TARGET xprintf
	PROPERTY COMPILE_FLAGS "${LOADER_COMPILE_FLAGS}")
SET_PROPERTY(TARGET xprintf
	PROPERTY LINK_FLAGS "${LOADER_LINK_FLAGS}")

ADD_LIBRARY(dummy STATIC dummy.S)
SET_PROPERTY(TARGET dummy 
	PROPERTY COMPILE_FLAGS "${LOADER_COMPILE_FLAGS}")
SET_PROPERTY(TARGET dummy
	PROPERTY LINK_FLAGS "${LOADER_LINK_FLAGS}")

