INCLUDE_DIRECTORIES(${SNITCHASER_CURRENT_SOURCE_DIR})

SET(SRC
	loader.c
	load_elf.c
	debug.c
	)
ADD_LIBRARY(interp SHARED
	${SRC})

TARGET_LINK_LIBRARIES(interp platform)

set_interp_properties(interp)

ADD_SUBDIRECTORY(mem)

# vim:ts=4:sw=4

