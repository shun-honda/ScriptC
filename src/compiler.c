#include "ast.h"
#include "compiler.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int id = 0;

ScriptCInstruction createInstruction(int op) {
  ScriptCInstruction inst = (ScriptCInstruction) malloc(sizeof(struct ScriptCInstruction));
  inst->op = op;
  return inst;
}

InstList createInstList(InstList prev, ScriptCInstruction inst) {
  InstList list = (InstList)malloc(sizeof(struct InstList));
  list->index = id++;
  list->prev = prev;
  list->next = NULL;
  list->inst = inst;
  return list;
}

static const char *get_opname(uint8_t opcode) {
  switch (opcode) {
#define OP_DUMPCASE(OP) \
  case I##OP:   \
    return "" #OP;
    IR_EACH(OP_DUMPCASE);
  default:
    assert(0 && "UNREACHABLE");
    break;
#undef OP_DUMPCASE
  }
  return "";
}

static void dumpInstruction(ScriptCInstruction inst) {
  fprintf(stderr, "[%d] %s ", id, get_opname(inst->op));
  switch (inst->op) {
#define OP_DUMPCASE(OP) case I##OP:
    OP_DUMPCASE(iconst) {
      fprintf(stderr, "%d", inst->int_val);
    }
  default:
    break;
  }
  fprintf(stderr, "\n");
}

static inline InstList convert(Node node, InstList list);

InstList convertNONE(Node node, InstList list) {
  return list;
}

InstList convertINT(Node node, InstList list) {
  ScriptCInstruction inst = createInstruction(Iiconst);
  inst->int_val = node->int_val;
  dumpInstruction(inst);
  return createInstList(list, inst);
}

InstList convertFLOAT(Node node, InstList list) {
  ScriptCInstruction inst = createInstruction(Idconst);
  inst->double_val = node->double_val;
  dumpInstruction(inst);
  return createInstList(list, inst);
}

InstList convertSTRING(Node node, InstList list) {
  ScriptCInstruction inst = createInstruction(Isconst);
  inst->string = node->string;
  dumpInstruction(inst);
  return createInstList(list, inst);
}

InstList convertBOOL(Node node, InstList list) {
  ScriptCInstruction inst = createInstruction(Idconst);
  inst->bool_val = node->bool_val;
  dumpInstruction(inst);
  return createInstList(list, inst);
}

InstList convertADD(Node node, InstList list) {
  return NULL;
}

InstList convertSUB(Node node, InstList list) {
  return NULL;
}

InstList convertMUL(Node node, InstList list) {
  return NULL;
}

InstList convertDIV(Node node, InstList list) {
  return NULL;
}

InstList convertPLUS(Node node, InstList list) {
  return NULL;
}

InstList convertMINUS(Node node, InstList list) {
  return NULL;
}

InstList convertFUNCDEF(Node node, InstList list) {
  return NULL;
}

InstList convertARGS(Node node, InstList list) {
  return NULL;
}

InstList convertSTATEMENTLIST(Node node, InstList list) {
  return NULL;
}

InstList convertNAME(Node node, InstList list) {
  return NULL;
}

InstList convertASSIGN(Node node, InstList list) {
  return NULL;
}

InstList convertSOURCE(Node node, InstList list) {
  if(node->list) {
    ListEntry entry = node->list->elements;
    for(;entry; entry = entry->next) {
      convert(entry->node, list);
    }
  }
  return list;
}

InstList convertFUNCCALL(Node node, InstList list) {
  return NULL;
}

InstList convertPRINT(Node node, InstList list) {
  return NULL;
}

InstList convertIF(Node node, InstList list) {
  return NULL;
}

InstList convertELSE(Node node, InstList list) {
  return NULL;
}

InstList convertWHILE(Node node, InstList list) {
  return NULL;
}

InstList convertBLOCK(Node node, InstList list) {
  return NULL;
}

InstList convertRETURN(Node node, InstList list) {
  return NULL;
}

InstList convertBREAK(Node node, InstList list) {
  return NULL;
}

InstList convertCONTINUE(Node node, InstList list) {
  return NULL;
}

InstList convertFOR(Node node, InstList list) {
  return NULL;
}

InstList convertLT(Node node, InstList list) {
  return NULL;
}

InstList convertGT(Node node, InstList list) {
  return NULL;
}

InstList convertLE(Node node, InstList list) {
  return NULL;
}

InstList convertGE(Node node, InstList list) {
  return NULL;
}

InstList convertEQ(Node node, InstList list) {
  return NULL;
}

InstList convertASSIGNADD(Node node, InstList list) {
  return NULL;
}

InstList convertASSIGNSUB(Node node, InstList list) {
  return NULL;
}

InstList convertASSIGNMUL(Node node, InstList list) {
  return NULL;
}

InstList convertASSIGNDIV(Node node, InstList list) {
  return NULL;
}

InstList convertINC(Node node, InstList list) {
  return NULL;
}

InstList convertDEC(Node node, InstList list) {
  return NULL;
}

ScriptCInstruction* createISeq(InstList list) {
  return NULL;
}

typedef InstList (*convert_to_lir_func_t)(Node, InstList);
static convert_to_lir_func_t f_convert[] = {
#define DEFINE_CONVERT_FUNC(NODE) convert##NODE,
  NODE_EACH(DEFINE_CONVERT_FUNC)
};

static inline InstList convert(Node node, InstList list) {
  return f_convert[node->type](node, list);
}

ScriptCInstruction* compile(Node node, InstList list) {
  f_convert[node->type](node, list);
  return createISeq(list);
}
