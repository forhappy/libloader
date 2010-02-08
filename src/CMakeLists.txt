INCLUDE_DIRECTORIES(${SNITCHASER_BINARY_DIR}/src
	${SNITCHASER_BINARY_DIR}
	${SNITCHASER_SOURCE_DIR}/src
	${SNITCHASER_SOURCE_DIR}/src/arch/${ARCH}
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

MACRO(set_interp_properties target)
SET_PROPERTY(TARGET ${target}
	APPEND
	PROPERTY COMPILE_FLAGS "-fvisibility=hidden -fPIC")
SET_PROPERTY(TARGET ${target}
	APPEND
	PROPERTY LINK_FLAGS "-nostartfiles -nodefaultlibs -nostdlib")
ENDMACRO(set_interp_properties target)

ADD_SUBDIRECTORY(arch)
ADD_SUBDIRECTORY(interp)
ADD_SUBDIRECTORY(host)
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(tests)

