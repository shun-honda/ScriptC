#ifndef __AST__
#define __AST__

#define TINYC_NONE 0
#define TINYC_INT 1
#define TINYC_FLOAT 2
#define TINYC_STRING 3
#define TINYC_BOOL 4
#define TINYC_ADD 5
#define TINYC_SUB 6
#define TINYC_MUL 7
#define TINYC_DIV 8
#define TINYC_PLUS 9
#define TINYC_MINUS 10
#define TINYC_FUNCDEF 11
#define TINYC_ARGS 12
#define TINYC_STATEMENTLIST 13
#define TINYC_NAME 14
#define TINYC_ASSIGN 15
#define TINYC_SOURCE 16
#define TINYC_FUNCCALL 17
#define TINYC_PRINT 18
#define TINYC_IF 19
#define TINYC_ELSE 20
#define TINYC_WHILE 21
#define TINYC_BLOCK 22
#define TINYC_RETURN 23
#define TINYC_BREAK 24
#define TINYC_CONTINUE 25
#define TINYC_FOR 26
#define TINYC_LT 27
#define TINYC_GT 28
#define TINYC_LE 29
#define TINYC_GE 30
#define TINYC_EQ 31
#define TINYC_ASSIGNADD 32
#define TINYC_ASSIGNSUB 33
#define TINYC_ASSIGNMUL 34
#define TINYC_ASSIGNDIV 35
#define TINYC_INC 36
#define TINYC_DEC 37

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
