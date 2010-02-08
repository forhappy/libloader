SET(SRC
	scan_opcode.c
	x86_opcode.c.h
	x86_opcode.h
	)

ADD_LIBRARY(platform STATIC ${SRC})
SET_PROPERTY(TARGET platform
	APPEND
	PROPERTY COMPILE_FLAGS "-fPIC")

# vim:ts=4:sw=4

