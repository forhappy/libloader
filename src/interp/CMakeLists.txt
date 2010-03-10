INCLUDE_DIRECTORIES(${SNITCHASER_CURRENT_SOURCE_DIR})

SET(SRC
	main.c
	auxv.c
	auxv.h
	loader.c
	mm.h
	mm.c
	)
ADD_LIBRARY(interp SHARED
	${SRC})

ADD_LIBRARY(interp_common STATIC
	debug.c)

TARGET_LINK_LIBRARIES(interp interp_common platform)

set_interp_properties(interp)
set_interp_properties(interp_common)

# ADD_SUBDIRECTORY(mem)

# vim:ts=4:sw=4

