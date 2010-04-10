SET(SRC
	main.c
	debug.c
	)

ADD_EXECUTABLE(snitchaser ${SRC})
TARGET_LINK_LIBRARIES(snitchaser)

# vim:ts=4:sw=4

