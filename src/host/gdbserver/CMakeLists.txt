SET(SRC
	event-loop.c
	hostio.c
	hostio-errno.c
	i386-low.c
	i387-fp.c
	inferiors.c
	linux-x86-low.c
	mem-break.c
	proc-service.c
	regcache.c
	remote-utils.c
	server.c
	signals.c
	target.c
	thread-db.c
	utils.c
	linux-low.c
	version.c
	reg-i386-linux.c

	snitchaser_patch.h
	snitchaser_patch.c

	)

IF (HAVE_THREAD_DB)
	SET_PROPERTY(SOURCE linux-low.c
		APPEND
		PROPERTY COMPILE_DEFINITIONS "USE_THREAD_DB")
ENDIF (HAVE_THREAD_DB)

ADD_LIBRARY(gdbserver STATIC ${SRC})
TARGET_LINK_LIBRARIES(gdbserver ${THREADDB_LIBS})

# vim:ts=4:sw=4

