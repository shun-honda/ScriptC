%{
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "ast.h"
#include "compiler.h"
#include "vm.h"
#define YYDEBUG 1

Node ast;
int yyerror(char const *str);

int yylex(void);

%}

%union {
  Node node;
}

%start Program
%token DEF PRINT IF ELSE WHILE RETURN BREAK CONTINUE FOR
%token LE GE EQ NE ADDEQ SUBEQ MULEQ DIVEQ INC DEC
%token<node> IDENTIFIER NONE TRUE FALSE INT FLOAT STRING

%type<node> Program Source
%type<node> Statement ExpressionStatement SimpleStatement
%type<node> PrintStatement ReturnStatement
%type<node> CompoundStatement IfStatement WhileStatement ForStatement Block
%type<node> BreakStatement ContinueStatement
%type<node> FunctionDefinition Arguments FunctionBody StatementList
%type<node> Expression AssignmentExpression CompExpression PostfixExpression
%type<node> ArithExpr Term Factor
%type<node> Literal NumericLiteral NullLiteral BooleanLiteral StringLiteral
%type<node> FunctionCall CallArgs

%%

Program
  : Source { ast = $1;}
  ;

Source
  : Statement {$$ = createListNode(SC_SOURCE, $1);}
  | Source Statement { appendList($1->list, $2); $$ = $1; }
  ;

Statement
  : FunctionDefinition {$$ = $1;}
  | ExpressionStatement {$$ = $1;}
  | SimpleStatement {$$ = $1;}
  | CompoundStatement {$$ = $1;}
  ;

SimpleStatement
  : PrintStatement {$$ = $1;}
  | ReturnStatement {$$ = $1;}
  | BreakStatement {$$ = $1;}
  | ContinueStatement {$$ = $1;}
  ;

ReturnStatement
  : RETURN Expression ';' {$$ = createReturnNode($2);}
  ;

PrintStatement
  : PRINT Expression ';' { $$ = createPrintNode($2); }
  ;

BreakStatement
  : BREAK ';' {$$ = createNode(SC_BREAK);}
  ;

ContinueStatement
  : CONTINUE ';' {$$ = createNode(SC_CONTINUE);}
  ;

CompoundStatement
  : IfStatement {$$ = $1;}
  | WhileStatement {$$ = $1;}
  | ForStatement {$$ = $1;}
  ;

IfStatement
  : IF Expression Block ELSE Block {$$ = createIfNode($2, $3, $5);}
  | IF Expression Block ELSE IfStatement {$$ = createIfNode($2, $3, $5);}
  | IF Expression Block {$$ = createIfNode($2, $3, createNode(SC_NONE));}
  ;

WhileStatement
  : WHILE Expression Block {$$ = createWhileNode($2, $3);}
  ;

ForStatement
  : FOR '(' ExpressionStatement ExpressionStatement ')' Block {$$ = createForNode($3, $4, NULL, $6);}
  | FOR '(' ExpressionStatement ExpressionStatement Expression ')' Block {$$ = createForNode($3, $4, $5, $7);}
  ;

Block
  : '{' StatementList '}' {$$ = createBlockNode($2);}
  ;

FunctionDefinition
  : DEF IDENTIFIER '(' Arguments ')' FunctionBody {$$ = createFuncDefNode($2, $4, $6);}
  ;

FunctionBody
  : '{' '}' {$$ = NULL;}
  | '{' StatementList '}' {$$ = $2;}
  ;

StatementList
  : Statement {$$ = createListNode(SC_STATEMENTLIST, $1);}
  | StatementList Statement { appendList($1->list, $2); $$ = $1; }
  ;

Arguments
  : IDENTIFIER {$$ = createListNode(SC_ARGS, $1);}
  | Arguments ',' IDENTIFIER { appendList($1->list, $3); $$ = $1; }
  ;

ExpressionStatement
  : ';' {$$ = NULL;}
  | Expression ';' {$$ = $1;}
  ;

Expression
  : AssignmentExpression {$$ = $1;}
  ;

AssignmentExpression
  : AssignmentExpression '=' CompExpression {$$ = createExprNode(SC_ASSIGN, $1, $3);}
  | AssignmentExpression ADDEQ CompExpression {$$ = createExprNode(SC_ASSIGNADD, $1, $3);}
  | AssignmentExpression SUBEQ CompExpression {$$ = createExprNode(SC_ASSIGNSUB, $1, $3);}
  | AssignmentExpression MULEQ CompExpression {$$ = createExprNode(SC_ASSIGNMUL, $1, $3);}
  | AssignmentExpression DIVEQ CompExpression {$$ = createExprNode(SC_ASSIGNDIV, $1, $3);}
  | CompExpression {$$ = $1;}
  ;

CompExpression
  : CompExpression '<' ArithExpr {$$ = createExprNode(SC_LT, $1, $3);}
  | CompExpression '>' ArithExpr {$$ = createExprNode(SC_GT, $1, $3);}
  | CompExpression EQ ArithExpr {$$ = createExprNode(SC_EQ, $1, $3);}
  | CompExpression NE ArithExpr {$$ = createExprNode(SC_NE, $1, $3);}
  | CompExpression LE ArithExpr {$$ = createExprNode(SC_LE, $1, $3);}
  | CompExpression GE ArithExpr {$$ = createExprNode(SC_GE, $1, $3);}
  | ArithExpr {$$ = $1;}
  ;

ArithExpr
  : ArithExpr '+' Term {$$ = createArithNode(SC_ADD, $1, $3);}
  | ArithExpr '-' Term {$$ = createArithNode(SC_SUB, $1, $3);}
  | Term {$$ = $1;}
  ;

Term
  : Term '*' Factor {$$ = createArithNode(SC_MUL, $1, $3);}
  | Term '/' Factor {$$ = createArithNode(SC_DIV, $1, $3);}
  | Factor {$$ = $1;}
  ;

Factor
  : PostfixExpression {$$ = $1;}
  | '+' Factor {$$ = createUnaryNode(SC_PLUS, $2);}
  | '-' Factor {$$ = createUnaryNode(SC_MINUS, $2);}
  ;

PostfixExpression
  : PostfixExpression INC {$$ = createUnaryNode(SC_INC, $1);}
  | PostfixExpression DEC {$$ = createUnaryNode(SC_DEC, $1);}
  | Literal {$$ = $1;}
  ;

Literal
  : NullLiteral {$$ = $1;}
  | BooleanLiteral {$$ = $1;}
  | NumericLiteral {$$ = $1;}
  | StringLiteral {$$ = $1;}
  | IDENTIFIER
  | FunctionCall {$$ = $1;}
  | '(' Expression ')' {$$ = $2;}
  ;

NullLiteral
  : NONE
  ;

BooleanLiteral
  : TRUE
  | FALSE
  ;

NumericLiteral
  : INT
  | FLOAT
  ;

StringLiteral
  : STRING
  ;

FunctionCall
  : IDENTIFIER '(' CallArgs ')' {$$ = createFuncCallNode($1, $3);}

CallArgs
  : AssignmentExpression {$$ = createListNode(SC_ARGS, $1);}
  | CallArgs ',' AssignmentExpression { appendList($1->list, $3); $$ = $1; }
  ;

%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}

int main(int argc, char *const argv[])
{
    extern int yyparse(void);
    extern FILE *yyin;
    extern Node ast;
    const char *input_file = NULL;
    int input_size = 0;
    const char *orig_argv0 = argv[0];
    int opt;

    while ((opt = getopt(argc, argv, "p:i:t:o:c:h:")) != -1) {
      switch (opt) {
        case 'i':
          input_file = optarg;
      		if (!(yyin = fopen(input_file, "r"))) {
      			fprintf(stderr, "File [%s] is not found!\n", argv[1]);
      			return 1;
      		}
          break;
        case 'h':
          printf("help");
          return 0;
        default: /* '?' */
          yyin = stdin;
          break;
      }
    }

    if (yyparse()) {
        fprintf(stderr, "Error ! Error ! Error !\n");
        exit(1);
    }

    printNode(ast, 0);
    createModule();
    CompilerContext cctx = createCompilerContext(NULL);
    ScriptCInstruction insts = compile(ast);
    VMContext ctx = createVMContext(NULL, 0);
    prepareVM(ctx, insts, cctx->code_length);
    vm_execute(ctx, insts);
    disposeNode(ast);
    return 0;
}
