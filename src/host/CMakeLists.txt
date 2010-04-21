SET(SRC
	main.c
	debug.c
	opts.c
	ptrace.h
	procmaps.h
	exception.h
	exception.c
	mm.h
	)

ADD_EXECUTABLE(snitchaser ${SRC})
TARGET_LINK_LIBRARIES(snitchaser platform_host)

# vim:ts=4:sw=4

