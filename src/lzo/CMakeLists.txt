SET(SRC
	lzo1x_compress.c
	lzo1x_decompress.c
	lzodefs.h
	)

ADD_LIBRARY(lzo STATIC ${SRC})

set_interp_properties(lzo)

# vim:ts=4:sw=4

