%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "x86_instructions.tbl.h"
#include "parser.tab.h"

#ifndef YYTOKENTYPE
enum tokentype {
	TK_HEXNUMBER = 256,
	TK_TABLE,
	TK_TABLENAME,
	TK_X32,
	TK_X64,
	TK_OPERATOR,
	TK_SPECIAL,
	TK_OPERADE,
	TK_REGNAME,
	TK_CONSTANT,
	TK_HINT,
	ERROR	= -1,
};
#endif

int line_counter = 0;

static int hextonum(const char *str)
{
	assert(str[0] == '0');
	assert(str[1] == 'x');
	int a, b;
	if ((str[2] >= 'a') && (str[2] <= 'f'))
		a = 10 + str[2] - 'a';
	else
		a = str[2] - '0';

	if ((str[3] >= 'a') && (str[3] <= 'f'))
		b = 10 + str[3] - 'a';
	else
		b = str[3] - '0';

	return a * 16 + b;
}

%}
DIGIT		[0-9]
HEXDIGIT	[0-9abcdef]
LLETTER		[a-z]
ULETTER		[A-Z]
LETTER		[a-zA-Z]
GLYPH		[a-zA-Z0-9_-]
%x COMMENT DESC OPBASE REGNAME OPHINTS CONSTANT
%option stack 

%%

"\n"				line_counter ++;
table				return TK_TABLE;
{LLETTER}{GLYPH}*	yylval.token = yytext; return TK_TABLENAME;
"{"					yy_push_state(DESC); return '{';
<INITIAL,DESC,OPBASE,OPHINTS>[ \t]	/* empty */
<INITIAL,DESC,OPBASE,OPHINTS>"#"		yy_push_state(COMMENT);
.					return ERROR;



<DESC>"X32"				return TK_X32;
<DESC>"X64"				return TK_X64;
<DESC>"0x"{HEXDIGIT}+		yylval.val = hextonum(yytext); return TK_HEXNUMBER;
<DESC>":"					yy_push_state(OPBASE); return ':';
<DESC>"}"				yy_pop_state(); return '}';
<DESC>"{"				yy_push_state(DESC); return '{';
<DESC>"-"				return '-';
<DESC>"\n"				line_counter ++;
<DESC>.					return ERROR;

<OPBASE,OPHINTS>\n			line_counter ++; yy_pop_state();

<OPBASE>{LLETTER}+3{0,1}			yylval.token = yytext; return TK_OPERATOR;
<OPBASE>","					return ',';
<OPBASE>{ULETTER}+{DIGIT}*+A{0,1}	yylval.token = yytext; return TK_SPECIAL;
<OPBASE>{ULETTER}{LLETTER}{1,2}	yylval.token = yytext; return TK_OPERADE;
<OPBASE>\"			yy_push_state(REGNAME); return '"';
<OPBASE>"|"			BEGIN(OPHINTS); return '|';
<OPBASE>"'"					yy_push_state(CONSTANT);
<CONSTANT>{DIGIT}+		yylval.token = yytext;	return TK_CONSTANT;
<CONSTANT>"'"				yy_pop_state();
<OPBASE>.					return ERROR;

<REGNAME>{GLYPH}+ 	yylval.token = yytext; return TK_REGNAME;
<REGNAME>"/"		return '/';
<REGNAME>\"			yy_pop_state(); return '"';
<REGNAME>.			return ERROR;

<OPHINTS>{GLYPH}+	yylval.token = yytext; return TK_HINT;
<OPHINTS>","		return ',';
<OPHINTS>"!"		return '!';
<OPHINTS>.			return ERROR;

<COMMENT>.			/* empty */
<COMMENT>\n			line_counter ++; yy_pop_state();

%%

#ifdef TEST_LEXER

int main()
{
	int res;
	while (res = yylex()) { 
		printf("xxx, %d, %s\n", res, yytext);
		if (res == -1) {
			printf("ERROR!!!\n");
			exit(-1);
		}
	}
	return 0;
}

#endif
// vim:ts=4:sw=4

