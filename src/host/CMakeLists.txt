SET(SRC
	main.c
	debug.c
	opts.c
	ptrace.h
	ptrace.c
	procmaps.h
	procmaps.c
	exception.h
	exception.c
	mm.h
	)

ADD_EXECUTABLE(snitchaser ${SRC})
TARGET_LINK_LIBRARIES(snitchaser)

# vim:ts=4:sw=4

