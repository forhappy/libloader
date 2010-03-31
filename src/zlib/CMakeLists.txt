SET(SRC
	deflate.c
	deflate_syms.c
	deftree.c
	inffast.c
	inflate.c
	inflate_syms.c
	inftrees.c
	infutil.c
	zlib.h
	)

ADD_LIBRARY(zlib STATIC ${SRC})

set_interp_properties(interp_common)

# vim:ts=4:sw=4

