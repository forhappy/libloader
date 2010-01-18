%{
#include <stdio.h>
#include <stdlib.h>

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

%}
DIGIT		[0-9]
HEXDIGIT	[0-9abcdef]
LLETTER		[a-z]
ULETTER		[A-Z]
LETTER		[a-zA-Z]
GLYPH		[a-zA-Z0-9]
%x COMMENT DESC OPBASE REGNAME OPHINTS
%option debug stack

%%

"\n"				line_counter ++;
table				return TK_TABLE;
{LLETTER}{GLYPH}*			return TK_TABLENAME;
"{"					yy_push_state(DESC); return '{';
<INITIAL,DESC,OPBASE,OPHINTS>[ \t]	/* empty */
<INITIAL,DESC,OPBASE,OPHINTS>"#"		yy_push_state(COMMENT);
.					return ERROR;



<DESC>"X32"				return TK_X32;
<DESC>"X64"				return TK_X64;
<DESC>"0x"{HEXDIGIT}+		return TK_HEXNUMBER;
<DESC>":"					yy_push_state(OPBASE); return ':';
<DESC>"}"				yy_pop_state(); return '}';
<DESC>"{"				yy_push_state(DESC); return '{';
<DESC>"-"				return '-';
<DESC>.					return ERROR;

<OPBASE,OPHINTS>\n			line_counter ++; yy_pop_state();

<OPBASE>{LLETTER}+			return TK_OPERATOR;
<OPBASE>","					return ',';
<OPBASE>{ULETTER}+{DIGIT}*	return TK_SPECIAL;
<OPBASE>{ULETTER}{LLETTER}{1,2}	return TK_OPERADE;
<OPBASE>\"			yy_push_state(REGNAME); return '"';
<OPBASE>"|"			BEGIN(OPHINTS); return '|';
<OPBASE>"'"{DIGIT}+"'"		return TK_CONSTANT;
<OPBASE>.					return ERROR;

<REGNAME>{GLYPH}+	return TK_REGNAME;
<REGNAME>"/"		return '/';
<REGNAME>\"			yy_pop_state(); return '"';
<REGNAME>.			return ERROR;

<OPHINTS>{GLYPH}+	return TK_HINT;
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

