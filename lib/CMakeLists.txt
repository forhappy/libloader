SET(SRC
	string.c
	vsprintf.c
	string_helpers.c
	ctype.c
	hexdump.c
	string_32.c
	memcpy_32.c
	)

ADD_LIBRARY(platform_lib STATIC ${SRC})
set_interp_properties(platform_lib)

ADD_LIBRARY(asm_platform_lib STATIC dummy.S)
set_interp_properties_asm(asm_platform_lib)

