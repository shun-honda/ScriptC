#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node createNode(int type) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = type;
  return node;
}

Node createFuncDefNode(Node name, Node args, Node body) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_FUNCDEF;
  node->child_size = 3;
  node->child = (Node *) calloc(sizeof(struct Node), 3);
  node->child[0] = name;
  node->child[1] = args;
  node->child[2] = body;
  return node;
}

Node createListNode(int type, Node child) {
  List list = createList(createListEntry(child, NULL));
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = type;
  node->list = list;
  return node;
}

List createList(ListEntry entry) {
  List list = (List) malloc(sizeof(struct List));
  list->elements = entry;
  list->cur = entry;
  return list;
}

List appendList(List list, Node node) {
  if(list->elements == NULL) {
    list->elements = createListEntry(node, NULL);
    list->cur = list->elements;
    return list;
  }
  list->cur = createListEntry(node, list->cur);
  return list;
}

ListEntry createListEntry(Node node, ListEntry prev) {
  ListEntry entry = (ListEntry) malloc(sizeof(struct ListEntry));
  entry->node = node;
  entry->prev = prev;
  entry->next = NULL;
  if(prev) {
    prev->next = entry;
  }
  return entry;
}

void disposeList(List list) {
  disposeListEntry(list->elements);
  free(list);
}

void disposeListEntry(ListEntry entry) {
  if(!entry) {
    return;
  }
  disposeListEntry(entry->next);
  disposeNode(entry->node);
  free(entry);
}

void printList(List list, int level) {
  if(list) {
    ListEntry entry = list->elements;
    for(;entry; entry = entry->next) {
      printNode(entry->node, level);
    }
  }
}

Node createExprNode(int type, Node left, Node right) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = type;
  node->child_size = 2;
  node->child = (Node *) calloc(sizeof(struct Node), 2);
  node->child[0] = left;
  node->child[1] = right;
  return node;
}

Node createArithNode(int type, Node left, Node right) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = type;
  node->child_size = 2;
  node->child = (Node *) calloc(sizeof(struct Node), 2);
  node->child[0] = left;
  node->child[1] = right;
  return node;
}

Node createUnaryNode(int type, Node child) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = type;
  node->child_size = 1;
  node->child = (Node *) calloc(sizeof(struct Node), 1);
  node->child[0] = child;
  return node;
}

Node createIntNode(int val) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_INT;
  node->int_val = val;
  return node;
}

Node createFloatNode(double val) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_FLOAT;
  node->double_val = val;
  return node;
}

Node createBoolNode(int val) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_BOOL;
  node->bool_val = val;
  return node;
}

Node createStringNode(char* str) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_STRING;
  node->string = (char *) malloc(strlen(str));
  strcpy(node->string, str);
  return node;
}

Node createNameNode(const char* name) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_NAME;
  size_t len = strlen(name)+1;
  node->name = (char *) malloc(len);
  strcpy(node->name, name);
  node->name[len-1] = 0;
  return node;
}

Node createFuncCallNode(Node name, Node args) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_FUNCCALL;
  node->child_size = 2;
  node->child = (Node *) calloc(sizeof(struct Node), 2);
  node->child[0] = name;
  node->child[1] = args;
  return node;
}

Node createPrintNode(Node child) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_PRINT;
  node->child_size = 1;
  node->child = (Node *) calloc(sizeof(struct Node), 1);
  node->child[0] = child;
  return node;
}

Node createIfNode(Node cond, Node thenStmt, Node elseStmt) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_IF;
  node->child_size = 3;
  node->child = (Node *) calloc(sizeof(struct Node), 3);
  node->child[0] = cond;
  node->child[1] = thenStmt;
  node->child[2] = elseStmt;
  return node;
}

Node createWhileNode(Node cond, Node block) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_WHILE;
  node->child_size = 2;
  node->child = (Node *) calloc(sizeof(struct Node), 2);
  node->child[0] = cond;
  node->child[1] = block;
  return node;
}

Node createForNode(Node first, Node second, Node third, Node block) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_FOR;
  node->child_size = 3;
  node->child = (Node *) calloc(sizeof(struct Node), 3);
  node->child[0] = first;
  node->child[1] = second;
  node->child[2] = third;
  node->child[3] = block;
  return node;
}

Node createBlockNode(Node child) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_BLOCK;
  node->child_size = 1;
  node->child = (Node *) calloc(sizeof(struct Node), 1);
  node->child[0] = child;
  return node;
}

Node createReturnNode(Node child) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = SC_RETURN;
  node->child_size = 1;
  node->child = (Node *) calloc(sizeof(struct Node), 1);
  node->child[0] = child;
  return node;
}

void disposeNode(Node node) {
  if(node) {
    switch (node->type) {
      case SC_SOURCE:
        disposeList(node->list);
        break;
      case SC_STRING:
        free(node->string);
        break;
      case SC_ADD:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_SUB:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_MUL:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_DIV:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_PLUS:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_MINUS:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_ASSIGNADD:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_ASSIGNSUB:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_ASSIGNMUL:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_ASSIGNDIV:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_INC:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_DEC:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_LT:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_GT:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_LE:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_GE:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_EQ:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_NE:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_FUNCDEF:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        disposeNode(node->child[2]);
        free(node->child);
        break;
      case SC_ARGS:
        disposeList(node->list);
        break;
      case SC_STATEMENTLIST:
        disposeList(node->list);
        break;
      case SC_NAME:
        free(node->name);
        break;
      case SC_ASSIGN:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_FUNCCALL:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_PRINT:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_IF:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        disposeNode(node->child[2]);
        free(node->child);
        break;
      case SC_WHILE:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        free(node->child);
        break;
      case SC_FOR:
        disposeNode(node->child[0]);
        disposeNode(node->child[1]);
        disposeNode(node->child[2]);
        disposeNode(node->child[3]);
        free(node->child);
        break;
      case SC_BLOCK:
        disposeNode(node->child[0]);
        free(node->child);
        break;
      case SC_RETURN:
        disposeNode(node->child[0]);
        free(node->child);
        break;
    }
    free(node);
  }
}

void indent(level) {
  for(int i = 0; i < level; i++) {
    printf("  ");
  }
}

void printNode(Node node, int level) {
  if(node) {
    indent(level);
    switch (node->type) {
      case SC_NONE:
        printf("#None[]\n");
        break;
      case SC_SOURCE:
        printf("#Source[\n");
        printList(node->list, level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_INT:
        printf("#Int[%d]\n", node->int_val);
        break;
      case SC_FLOAT:
        printf("#Float[%f]\n", node->double_val);
        break;
      case SC_STRING:
        printf("#String[%s]\n", node->string);
        break;
      case SC_BOOL:
        if(node->bool_val) {
          printf("#Bool[true]\n");
        }
        else {
          printf("#Bool[false]\n");
        }
        break;
      case SC_ADD:
        printf("#Add[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_SUB:
        printf("#Sub[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_MUL:
        printf("#Mul[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_DIV:
        printf("#Div[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_PLUS:
        printf("#Plus[\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_MINUS:
        printf("#Minus\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_ASSIGNADD:
        printf("#AddEq\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_ASSIGNSUB:
        printf("#SubEq\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_ASSIGNMUL:
        printf("#MulEq\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_ASSIGNDIV:
        printf("#DivEq\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_INC:
        printf("#Inc\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_DEC:
        printf("#Dec\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_LT:
        printf("#LT\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_GT:
        printf("#GT\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_LE:
        printf("#LE\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_GE:
        printf("#GE\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_EQ:
        printf("#EQ\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_NE:
        printf("#NE\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_FUNCDEF:
        printf("#FuncDef\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        printNode(node->child[2], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_ARGS:
        printf("#Args[\n");
        printList(node->list, level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_STATEMENTLIST:
        printf("#List[\n");
        printList(node->list, level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_NAME:
        printf("#Name[%s]\n", node->name);
        break;
      case SC_ASSIGN:
        printf("#Assign[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_FUNCCALL:
        printf("#FuncCall\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_PRINT:
        printf("#Print[\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_IF:
        printf("#If[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        printNode(node->child[2], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_WHILE:
        printf("#While\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_FOR:
        printf("#For[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        printNode(node->child[2], level+1);
        printNode(node->child[3], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_BLOCK:
        printf("#Block[\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_RETURN:
        printf("#Return[\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case SC_BREAK:
        printf("#Break[]\n");
        break;
      case SC_CONTINUE:
        printf("#Continue[]\n");
        break;
    }
  }
}
