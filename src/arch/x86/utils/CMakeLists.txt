INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	)

BISON_TARGET(parser
	parser.y
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
	)
FLEX_TARGET(lexer
	lexer.l
	${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
	)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)
ADD_EXECUTABLE(x86_tbl_parser
	${FLEX_lexer_OUTPUTS}
	${BISON_parser_OUTPUTS}
	x86_instructions.tbl.h
	x86_registers.c
	x86_registers.h
	buildtable.c
	)

SET_PROPERTY(TARGET x86_tbl_parser
	APPEND
	PROPERTY COMPILE_FLAGS "-D_FORTIFY_SOURCE=0")

TARGET_LINK_LIBRARIES(x86_tbl_parser ${FLEX_LIBRARIES})

