#ifndef __AST__
#define __AST__

#define SC_NONE 0
#define SC_INT 1
#define SC_FLOAT 2
#define SC_STRING 3
#define SC_BOOL 4
#define SC_ADD 5
#define SC_SUB 6
#define SC_MUL 7
#define SC_DIV 8
#define SC_PLUS 9
#define SC_MINUS 10
#define SC_FUNCDEF 11
#define SC_ARGS 12
#define SC_STATEMENTLIST 13
#define SC_NAME 14
#define SC_ASSIGN 15
#define SC_SOURCE 16
#define SC_FUNCCALL 17
#define SC_PRINT 18
#define SC_IF 19
#define SC_ELSE 20
#define SC_WHILE 21
#define SC_BLOCK 22
#define SC_RETURN 23
#define SC_BREAK 24
#define SC_CONTINUE 25
#define SC_FOR 26
#define SC_LT 27
#define SC_GT 28
#define SC_LE 29
#define SC_GE 30
#define SC_EQ 31
#define SC_NE 32
#define SC_ASSIGNADD 33
#define SC_ASSIGNSUB 34
#define SC_ASSIGNMUL 35
#define SC_ASSIGNDIV 36
#define SC_INC 37
#define SC_DEC 38

#define NODE_EACH(NODE)\
  NODE(NONE)\
  NODE(INT)\
  NODE(FLOAT)\
  NODE(STRING)\
  NODE(BOOL)\
  NODE(ADD)\
  NODE(SUB)\
  NODE(MUL)\
  NODE(DIV)\
  NODE(PLUS)\
  NODE(MINUS)\
  NODE(FUNCDEF)\
  NODE(ARGS)\
  NODE(STATEMENTLIST)\
  NODE(NAME)\
  NODE(ASSIGN)\
  NODE(SOURCE)\
  NODE(FUNCCALL)\
  NODE(PRINT)\
  NODE(IF)\
  NODE(ELSE)\
  NODE(WHILE)\
  NODE(BLOCK)\
  NODE(RETURN)\
  NODE(BREAK)\
  NODE(CONTINUE)\
  NODE(FOR)\
  NODE(LT)\
  NODE(GT)\
  NODE(LE)\
  NODE(GE)\
  NODE(EQ)\
  NODE(NE)\
  NODE(ASSIGNADD)\
  NODE(ASSIGNSUB)\
  NODE(ASSIGNMUL)\
  NODE(ASSIGNDIV)\
  NODE(INC)\
  NODE(DEC)

struct Node {
  int type;
  int child_size;
  union {
    struct Node **child;
    int int_val;
    double double_val;
    char* string;
    int bool_val;
    struct List* list;
    char* name;
  };
};

struct List {
  struct ListEntry* elements;
  struct ListEntry* cur; /* this variable is used to create list node */
};

struct ListEntry {
  struct ListEntry* prev;
  struct ListEntry* next;
  struct Node* node;
};

typedef struct Node *Node;
typedef struct List *List;
typedef struct ListEntry *ListEntry;


Node createNode(int type);
void disposeNode(Node node);
void printNode(Node node, int level);
Node createFuncDefNode(Node name, Node args, Node body);
Node createListNode(int type, Node child);
List createList(ListEntry entry);
ListEntry createListEntry(Node node, ListEntry prev);
void disposeList(List list);
void disposeListEntry(ListEntry entry);
List appendList(List list, Node node);
Node createExprNode(int type, Node left, Node right);
Node createArithNode(int type, Node left, Node right);
Node createUnaryNode(int type, Node child);
Node createIntNode(int val);
Node createFloatNode(double val);
Node createBoolNode(int val);
Node createStringNode(char* str);
Node createNameNode(const char* name);
Node createFuncCallNode(Node name, Node args);
Node createPrintNode(Node child);
Node createIfNode(Node cond, Node thenStmt, Node elseStmt);
Node createWhileNode(Node cond, Node block);
Node createForNode(Node first, Node second, Node third, Node block);
Node createBlockNode(Node child);
Node createReturnNode(Node child);

#endif
