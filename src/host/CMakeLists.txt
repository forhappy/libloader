SET(COMMONSRC
	debug.c
	exception.h
	exception.c
	)

SET(SN_SRC
	main.c
	opts.c
	sockpair.c
	ptrace.h
	procmaps.h
	mm.h
	elf.h
	)

SET(UC_SRC
	uncompress_log.c
	)

ADD_LIBRARY(common_host ${COMMONSRC})

ADD_SUBDIRECTORY(gdbserver)


ADD_EXECUTABLE(snitchaser ${SN_SRC})
ADD_EXECUTABLE(uncompress_log ${UC_SRC})
# This is a trick to force cmake list "-lthread_db" at the beginning of
# generated command line. This is because libthread_db rely on some symbols
# defined in gdbserver
TARGET_LINK_LIBRARIES(snitchaser ${THREADDB_LIBS} gdbserver common_host platform_host)
TARGET_LINK_LIBRARIES(uncompress_log common_host lzo)

################ wrapped gdbserver ################


# vim:ts=4:sw=4

