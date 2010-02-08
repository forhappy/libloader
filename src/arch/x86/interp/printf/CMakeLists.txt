SET(SRC
	vsprintf.c
	string.c
	__ctype.c
	)

ADD_LIBRARY(printf STATIC ${SRC})

set_interp_properties(printf)

# vim:ts=4:sw=4
