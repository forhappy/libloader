SET(SRC
	loader.c
	)
ADD_LIBRARY(interp SHARED
	${SRC})
SET_PROPERTY(TARGET interp
	APPEND
	PROPERTY COMPILE_FLAGS "-fvisibility=hidden")
SET_PROPERTY(TARGET interp
	APPEND
	PROPERTY LINK_FLAGS "-nostartfiles -nodefaultlibs -nostdlib")
# vim:ts=4:sw=4

