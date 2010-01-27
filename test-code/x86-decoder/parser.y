%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "x86_instructions.tbl.h"

#ifndef PARSER_DEBUG_OUT
# define printf(...)	do { } while(0)
#endif

static int nr_strdup = 0;
static int nr_free = 0;

static const char * table_name = NULL;

char *
xstrdup(const char * str)
{
	nr_strdup ++;
	return strdup(str);
}

void
xfree(void * ptr)
{
	nr_free ++;
	free(ptr);
}

#define strdup xstrdup
#define free xfree



extern int line_counter;
static void yyerror(const char *str)
{
	fprintf(stderr, "error at line %d: %s\n",
		line_counter, str);
	exit(1);
}

extern int yylex();

static void
begin_table(const char * tn)
{
	printf("begin table %s\n", tn);
	table_name = tn;
}

static void
end_table(const char * tn)
{
	printf("end table %s\n", tn);
	free((void*)tn);
}

static int nr_operades = 0;
static struct _operade operades[4];

static int nr_hints = 0;
static struct _hint hints[4];
static int jmpnote = 0;


static const char * operator = NULL;
static int table_head = 32;

static struct _entry_rng rng;

void
new_description()
{

	int _th = (table_head == 132) ? 32 : table_head;
	if (table_head == 132) {
		hints[nr_hints].hint = strdup("i64");
		nr_hints ++;
	}

	if (table_head == 64) {
		hints[nr_hints].hint = strdup("o64");
		nr_hints ++;
	}

	add_descriptor(
		table_name,
		_th,
		operator,
		rng,
		nr_operades,
		operades,
		nr_hints,
		hints,
		jmpnote);

	printf("%d:0x%x-0x%x %s", table_head, rng.start, rng.end,
		operator);
	free((void*)operator);
	for (int i = 0; i < nr_operades; i++) {
		struct _operade o = operades[i];
		if (o.type == _OPERADE_NORMAL) {
			printf(", %s", o.u.normal);
			free((void*)(o.u.normal));
		} else if (o.type == _OPERADE_CONSTANT) {
			printf(", '%d'", o.u.constant);
		} else {
			assert(o.type == _OPERADE_REGISTERS);
			printf(", \"%s/%s\"",
				o.u.regs.r1, o.u.regs.r2);
			if (o.u.regs.r1)
				free((void*)(o.u.regs.r1));
			if (o.u.regs.r2)
				free((void*)(o.u.regs.r2));
		}
	}

	if (nr_hints > 0) {
		printf(" | ");
		for (int i = 0; i < nr_hints; i++) {
			printf("%s, ", hints[i].hint);
			free((void*)hints[i].hint);
		}
	}

	if (jmpnote != 0)
		printf(" !");

	printf("\n");

	operator = NULL;
	nr_operades = 0;
	nr_hints = 0;
	jmpnote = 0;
}

%}

%debug
%union {
	char * token;
	char * string;
	int val;
	struct _regs regs;
}

%token <val> TK_HEXNUMBER
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

%type <token> table table_head normal_operade
%type <val>	blockhead xxoperades operades operade constant
%type <regs> regname

%start x86opc

%%
x86opc:		/* empty */
	  | tables
	  ;

tables: table
	  | table tables
	  ;



table: table_head '{' descriptors '}' { end_table($1); }
	 ;

table_head: TK_TABLE TK_TABLENAME	{ $$ = strdup($2); begin_table($$); }
		  ;

descriptors: descriptor
		   | descriptor descriptors
		   ;

descriptor: normal_descriptor
		  | block_descriptor
		  ;

block_descriptor: blockhead '{' descriptors '}'	{ table_head = $1;}
				;

blockhead: TK_X32 { $$ = table_head; table_head = 132; }
		 | TK_X64 { $$ = table_head; table_head = 64; }
		 ;

normal_descriptor: opcode_rng ':' operator operades hints jmpnote { new_description();}
				 ;

opcode_rng: TK_HEXNUMBER	{ rng.start = rng.end = $1; }
		  | TK_HEXNUMBER '-' TK_HEXNUMBER { rng.start = $1; rng.end = $3; }
		  ;

operator: TK_OPERATOR { operator_type = _OPERATOR_NORMAL; operator = strdup($1); }
		| TK_SPECIAL { operator_type = _OPERATOR_SPECIAL; operator = strdup($1);}
		;

operades:	 /* empty */	{ nr_operades = 0; }
		| xxoperades
		;

xxoperades: operade	{ nr_operades ++;}
		  | xxoperades ',' operade { nr_operades ++;}

operade: normal_operade	{
		   operades[nr_operades].type = _OPERADE_NORMAL;
		   operades[nr_operades].u.normal = $1;
	   }
	   | '"' regname '"' {
	   		operades[nr_operades].type = _OPERADE_REGISTERS;
			operades[nr_operades].u.regs = $2;
	   }
	   | constant        {
	   		operades[nr_operades].type = _OPERADE_CONSTANT;
			operades[nr_operades].u.constant = $1;
	   }
	   ;

normal_operade: TK_OPERADE 	{
			  					$$ = strdup($1);
			  				}
			  ;

regname: TK_REGNAME			{
	   							$$.r1 = strdup($1);
								$$.r2 = NULL;
	   						}
	   | regname '/' TK_REGNAME
	   						{
								$$ = $1;
								$$.r2 = strdup($3);
							}
	   ;

constant: TK_CONSTANT
		{
			$$ = atoi($1);
		}
		;

hints: 	/* empty */ { nr_hints = 0; }
	 | '|' xxhints
	 | '|'	{ nr_hints = 0; }
	 ;

xxhints: TK_HINT { hints[nr_hints].hint = strdup($1); nr_hints ++; }
	   | xxhints ',' TK_HINT { hints[nr_hints].hint = strdup($3), nr_hints ++; }
	   ;

jmpnote:	/* empty */
	   | '!' { jmpnote = 1; }
	   ;

%%

#ifdef TEST_PARSER
extern void print_table(void);
int main()
{
	yydebug = 0;
	yyparse();
	printf("nr_strdup = %d, nr_free = %d\n",
		nr_strdup, nr_free);
	print_table();
	return 0;
}
#endif

// vim:ts=4:sw=4

