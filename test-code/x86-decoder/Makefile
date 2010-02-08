all: test_scan_opcode

CFLAGS=-g -std=gnu99 -D_GNU_SOURCE -DTEST_PARSER

test_scan_opcode: test_scan_opcode.c scan_opcode.c  x86_opcode.h x86_opcode.c.h
	gcc $(CFLAGS) test_scan_opcode.c scan_opcode.c -o $@

x86_opcode.c.h: parser x86_instructions.tbl
	./parser < x86_instructions.tbl > $@

parser: x86_instructions.tbl.h lex.yy.c parser.tab.c x86_registers.c x86_registers.h buildtable.c
	gcc $(CFLAGS) lex.yy.c parser.tab.c x86_registers.c buildtable.c -lfl -o parser

parser.tab.c: parser.y
	bison -d $<
lex.yy.c: lexer.l
	flex $<

.PHONY: clean

clean:
	rm -f *.o parser.tab.c parser.tab.h lex.yy.c x86_opcode.c x86_opcode.c.h test_scan_opcode parser
