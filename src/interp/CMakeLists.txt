# INCLUDE_DIRECTORIES(${SNITCHASER_CURRENT_SOURCE_DIR})

SET(SRC
	main.c
	test.c
	auxv.c
	auxv.h
	loader.c
	mm.h
	mm.c
	dict.h
	dict.c
	code_cache.h
	code_cache.c
	logger.h
	logger.c
	compress.h
	compress.c
	checkpoint.h
	checkpoint.c
	)
ADD_LIBRARY(interp SHARED
	${SRC})
ADD_LIBRARY(xinterp SHARED
	${SRC})

SET_PROPERTY(TARGET interp
	APPEND
	PROPERTY COMPILE_DEFINITIONS "_START_SYMBOL=_start"
	)

SET_PROPERTY(TARGET xinterp
	APPEND
	PROPERTY COMPILE_DEFINITIONS "_START_SYMBOL=_xstart"
	)

ADD_LIBRARY(interp_common STATIC
	debug.c)

TARGET_LINK_LIBRARIES(interp interp_common platform zlib lzo)
TARGET_LINK_LIBRARIES(xinterp interp_common platform zlib lzo)



set_interp_properties(interp)
set_interp_properties(xinterp)
SET_PROPERTY(TARGET xinterp
	PROPERTY LINK_FLAGS "${INTERP_LINK_FLAGS} -Wl,--entry,_xstart")
set_interp_properties(interp_common)

# ADD_SUBDIRECTORY(mem)

# vim:ts=4:sw=4

