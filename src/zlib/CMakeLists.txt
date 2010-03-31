IF(USE_ZLIB)
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
ELSE(USE_ZLIB)
	SET(SRC
		dummy.c
		)
ENDIF(USE_ZLIB)
ADD_LIBRARY(zlib STATIC ${SRC})

set_interp_properties(zlib)

# vim:ts=4:sw=4

