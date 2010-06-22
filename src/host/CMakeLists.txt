SET(COMMONSRC
	debug.c
	exception.h
	exception.c
	)

SET(SN_SRC
	main.c
	snitchaser_opts.h
	snitchaser_opts.c
	sockpair.c
	ptrace.h
	procmaps.h
	mm.h
	elf.h
	)

SET(UC_SRC
	uncompress_log.c
	)

SET(tests
	test_replay_log)

ADD_LIBRARY(common_host ${COMMONSRC})
ADD_LIBRARY(replay_log replay_log.c replay_log.h)
TARGET_LINK_LIBRARIES(replay_log lzo)

ADD_SUBDIRECTORY(gdbserver)

ADD_EXECUTABLE(snitchaser ${SN_SRC})
ADD_EXECUTABLE(uncompress_log ${UC_SRC})
# This is a trick to force cmake list "-lthread_db" at the beginning of
# generated command line. This is because libthread_db rely on some symbols
# defined in gdbserver
TARGET_LINK_LIBRARIES(snitchaser ${THREADDB_LIBS} gdbserver common_host platform_host replay_log)
TARGET_LINK_LIBRARIES(uncompress_log common_host lzo)

build_tests("${tests}" "common_host;replay_log")

################ wrapped gdbserver ################


# vim:ts=4:sw=4

