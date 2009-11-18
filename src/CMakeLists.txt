INCLUDE_DIRECTORIES(${SNITCHASER_BINARY_DIR}/src
	${SNITCHASER_BINARY_DIR}
	${SNITCHASER_SOURCE_DIR}/src
	${SNITCHASER_SOURCE_DIR}/include/${ARCH}
	)

MACRO(build_tests testlist libs)
IF (BUILD_TESTS)
	FOREACH(t ${testlist})
		ADD_EXECUTABLE(${t} ${t}.c)
		FOREACH(l ${libs})
			TARGET_LINK_LIBRARIES(${t} ${l})
		ENDFOREACH(l ${libs})
	ENDFOREACH(t ${tests})
ENDIF(BUILD_TESTS)
ENDMACRO(build_tests testlist libs)


ADD_SUBDIRECTORY(common)

