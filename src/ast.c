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
  node->type = TINYC_FUNCDEF;
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
  node->type = TINYC_INT;
  node->int_val = val;
  return node;
}

Node createFloatNode(double val) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_FLOAT;
  node->double_val = val;
  return node;
}

Node createBoolNode(int val) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_BOOL;
  node->bool_val = val;
  return node;
}

Node createStringNode(char* str) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_STRING;
  node->string = (char *) malloc(strlen(str));
  strcpy(node->string, str);
  return node;
}

Node createNameNode(const char* name) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_NAME;
  node->name = (char *) malloc(strlen(name));
  strcpy(node->name, name);
  return node;
}

Node createFuncCallNode(Node name, Node args) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_FUNCCALL;
  node->child_size = 2;
  node->child = (Node *) calloc(sizeof(struct Node), 2);
  node->child[0] = name;
  node->child[1] = args;
  return node;
}

Node createPrintNode(Node child) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_PRINT;
  node->child_size = 1;
  node->child = (Node *) calloc(sizeof(struct Node), 1);
  node->child[0] = child;
  return node;
}

Node createIfNode(Node cond, Node thenStmt, Node elseStmt) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_IF;
  node->child_size = 3;
  node->child = (Node *) calloc(sizeof(struct Node), 3);
  node->child[0] = cond;
  node->child[1] = thenStmt;
  node->child[2] = elseStmt;
  return node;
}

Node createWhileNode(Node cond, Node block) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_WHILE;
  node->child_size = 2;
  node->child = (Node *) calloc(sizeof(struct Node), 2);
  node->child[0] = cond;
  node->child[1] = block;
  return node;
}

Node createForNode(Node first, Node second, Node third, Node block) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_FOR;
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
  node->type = TINYC_BLOCK;
  node->child_size = 1;
  node->child = (Node *) calloc(sizeof(struct Node), 1);
  node->child[0] = child;
  return node;
}

Node createReturnNode(Node child) {
  Node node = (Node) malloc(sizeof(struct Node));
  node->type = TINYC_RETURN;
  node->child_size = 1;
  node->child = (Node *) calloc(sizeof(struct Node), 1);
  node->child[0] = child;
  return node;
}

void disposeNode(Node node) {
  if(node) {
    int size = node->child_size;
    for(int i = 0; i < size; i++) {
      disposeNode(node->child[i]);
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
      case TINYC_SOURCE:
        printf("#Source[\n");
        printList(node->list, level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_INT:
        printf("#Int[%d]\n", node->int_val);
        break;
      case TINYC_FLOAT:
        printf("#Float[%f]\n", node->double_val);
        break;
      case TINYC_STRING:
        printf("#String[%s]\n", node->string);
        break;
      case TINYC_BOOL:
        if(node->bool_val) {
          printf("#Bool[true]\n");
        }
        else {
          printf("#Bool[false]\n");
        }
        break;
      case TINYC_ADD:
        printf("#Add[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_SUB:
        printf("#Sub[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_MUL:
        printf("#Mul[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_DIV:
        printf("#Div[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_PLUS:
        printf("#Plus[\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_MINUS:
        printf("#Minus\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_ASSIGNADD:
        printf("#AddEq\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_ASSIGNSUB:
        printf("#SubEq\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_ASSIGNMUL:
        printf("#MulEq\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_ASSIGNDIV:
        printf("#DivEq\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_INC:
        printf("#Inc\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_DEC:
        printf("#Dec\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_LT:
        printf("#LT\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_GT:
        printf("#GT\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_LE:
        printf("#LE\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_GE:
        printf("#GE\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_EQ:
        printf("#EQ\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_FUNCDEF:
        printf("#FuncDef\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        printNode(node->child[2], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_ARGS:
        printf("#Args[\n");
        printList(node->list, level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_STATEMENTLIST:
        printf("#List[\n");
        printList(node->list, level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_NAME:
        printf("#Name[%s]\n", node->name);
        break;
      case TINYC_ASSIGN:
        printf("#Assign[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_FUNCCALL:
        printf("#FuncCall\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_PRINT:
        printf("#Print[\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_IF:
        printf("#If[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        printNode(node->child[2], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_ELSE:
        break;
      case TINYC_WHILE:
        printf("#While\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_FOR:
        printf("#For[\n");
        printNode(node->child[0], level+1);
        printNode(node->child[1], level+1);
        printNode(node->child[2], level+1);
        printNode(node->child[3], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_BLOCK:
        printf("#Block[\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_RETURN:
        printf("#Return[\n");
        printNode(node->child[0], level+1);
        indent(level);
        printf("]\n");
        break;
      case TINYC_BREAK:
        printf("#Break[]\n");
        break;
      case TINYC_CONTINUE:
        printf("#Continue[]\n");
        break;
    }
  }
}
