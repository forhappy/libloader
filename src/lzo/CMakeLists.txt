IF(USE_LZO)
	SET(SRC
		lzo1x_compress.c
		lzo1x_decompress.c
		lzodefs.h
		)
ELSE(USE_LZO)
	SET(SRC
		dummy.c
		)
ENDIF(USE_LZO)

ADD_LIBRARY(lzo STATIC ${SRC})

set_interp_properties(lzo)

# vim:ts=4:sw=4

