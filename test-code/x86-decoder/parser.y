%{

#include <stdio.h>
#include <stdlib.h>

extern int line_counter;
static void yyerror(const char *str)
{
	fprintf(stderr, "error at line %d: %s\n",
		line_counter, str);
	exit(1);
}

extern int yylex();

%}

%debug
%union {
	char * token;
}

%token <token> TK_HEXNUMBER
%token <token> TK_TABLE
%token <token> TK_TABLENAME
%token <token> TK_X32
%token <token> TK_X64
%token <token> TK_OPERATOR
%token <token> TK_SPECIAL
%token <token> TK_OPERADE
%token <token> TK_REGNAME
%token <token> TK_CONSTANT
%token <token> TK_HINT
%token <token> ERROR

%start x86opc

%%
x86opc:		/* empty */
	  | tables
	  ;

tables: table
	  | table tables
	  ;



table: table_head '{' descriptors '}'
	 ;

table_head: TK_TABLE TK_TABLENAME
		  ;

descriptors: descriptor
		   | descriptor descriptors
		   ;

descriptor: normal_descriptor
		  | block_descriptor
		  ;

block_descriptor: blockhead '{' descriptors '}'
				;

blockhead: TK_X32
		 | TK_X64
		 ;

normal_descriptor: opcode_rng ':' operator operades hints jmpnote
				 ;

opcode_rng: TK_HEXNUMBER
		  | TK_HEXNUMBER '-' TK_HEXNUMBER
		  ;

operator: TK_OPERATOR
		| TK_SPECIAL
		;

operades:	/* empty */
		| xxoperades
		;

xxoperades: operade
		  | operade ',' xxoperades

operade: normal_operade
	   | '"' regname '"'
	   | constant
	   ;

normal_operade: TK_OPERADE
			  ;

regname: TK_REGNAME
	   | regname '/' TK_REGNAME
	   ;

constant: TK_CONSTANT
		;

hints:	/* empty */
	 | '|' xxhints
	 ;

xxhints: TK_HINT
	   | TK_HINT ',' xxhints
	   ;

jmpnote:	/* empty */
	   | '!'
	   ;

%%

#ifdef TEST_PARSER
int main()
{
	yydebug = 1;
	yyparse();
	return 0;
}
#endif

// vim:ts=4:sw=4

