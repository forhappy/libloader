# SET(SRC
# 	scan_opcode.c
# 	x86_opcode.c.h
# 	x86_opcode.h
# 	)
# 
# ADD_LIBRARY(platform STATIC ${SRC})
# SET_PROPERTY(TARGET platform
# 	APPEND
# 	PROPERTY COMPILE_FLAGS "-fPIC")

ADD_SUBDIRECTORY(host)
ADD_SUBDIRECTORY(interp)
ADD_SUBDIRECTORY(utils)

# vim:ts=4:sw=4

