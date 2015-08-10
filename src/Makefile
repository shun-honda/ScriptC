scriptC:	lex.yy.c y.tab.c
	gcc y.tab.c lex.yy.c ast.c compiler.c vm.c -o scriptC -g
y.tab.c:	scriptC.y
	yacc -dv scriptC.y
lex.yy.c:	scriptC.l
	lex scriptC.l
clean:
	rm y.tab.c lex.yy.c y.output y.tab.h scriptC
