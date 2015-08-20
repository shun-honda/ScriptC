%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "y.tab.h"

int yywrap(void) {
  return 1;
}

%}

%x SINGLE_STRING
%x DOUBLE_STRING

space [ \t\r\n]*

%%

"def" return DEF;

"(" return '(';
")" return ')';
"," return ',';
";" return ';';
"{" return '{';
"}" return '}';
"++" return INC;
"--" return DEC;
"+=" return ADDEQ;
"-=" return SUBEQ;
"*=" return MULEQ;
"/=" return DIVEQ;
"+" return '+';
"-" return '-';
"*" return '*';
"/" return '/';
"<=" return LE;
">=" return GE;
"==" return EQ;
"!=" return NE;
"<" return '<';
">" return '>';
"=" return '=';
"None" { yylval.node = createNode(SC_NONE); return NONE; }
"true" { yylval.node = createBoolNode(1); return TRUE;}
"false" { yylval.node = createBoolNode(0); return FALSE;}
"print" {return PRINT;}
"if" {return IF;}
"else" {return ELSE;}
"while" {return WHILE;}
"return" {return RETURN;}
"break" {return BREAK;}
"continue" {return CONTINUE;}
"for" {return FOR;}

{space} {}
(0|[1-9][0-9]*)"."[0-9]+ { yylval.node = createFloatNode(atof(yytext)); return FLOAT;}
0|[1-9][0-9]* { yylval.node = createIntNode(atoi(yytext)); return INT; }

\" { BEGIN DOUBLE_STRING; }
<DOUBLE_STRING>(\\.|[^\\"])* { yylval.node = createStringNode(yytext); return STRING;}
<DOUBLE_STRING>\" { BEGIN INITIAL; }

\' { BEGIN SINGLE_STRING; }
<SINGLE_STRING>(\\.|[^\\'])* { yylval.node = createStringNode(yytext); return STRING;}
<SINGLE_STRING>\' { BEGIN INITIAL; }

[a-zA-Z_][a-zA-Z0-9_]* {
  yylval.node = createNameNode(yytext);
  return IDENTIFIER;
}

%%
