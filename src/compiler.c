#include "ast.h"
#include "compiler.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int id = 0;

struct VarEntry {
  int id;
  char* name;
};

#define VAR_MAX 128
struct CompilerContext {
  struct VarEntry** vars;
  int var_count;
};

typedef struct CompilerContext* CompilerContext;
typedef struct VarEntry* VarEntry;

static CompilerContext c_context;

VarEntry getVarEntry(char* name) {
  for(int i = 0; i < c_context->var_count; i++) {
    if(!strcmp(name, c_context->vars[i]->name)) {
      return c_context->vars[i];
    }
  }
  return NULL;
}

void setVarEntry(char* name) {
  c_context->vars[c_context->var_count] = (VarEntry)malloc(sizeof(struct VarEntry));
  c_context->vars[c_context->var_count]->name = name;
  c_context->vars[c_context->var_count]->id = c_context->var_count;
  c_context->var_count++;
  if((c_context->var_count % VAR_MAX) == 0) {
    c_context->vars = (VarEntry*)realloc(c_context->vars, sizeof(c_context->vars)*2);
  }
}

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
  if(prev != NULL) {
    prev->next = list;
  }
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

static void dumpInstruction(ScriptCInstruction inst, int index) {
  fprintf(stderr, "[%d] %s ", index, get_opname(inst->op));
  switch (inst->op) {
#define OP_DUMPCASE(OP) case I##OP:
    OP_DUMPCASE(iconst) {
      fprintf(stderr, "%d", inst->int_val);
      break;
    }
    OP_DUMPCASE(dconst) {
      fprintf(stderr, "%f", inst->double_val);
      break;
    }
    OP_DUMPCASE(sconst) {
      fprintf(stderr, "%s", inst->string);
      break;
    }
    OP_DUMPCASE(bconst) {
      if(inst->bool_val) {
        fprintf(stderr, "true");
      } else {
        fprintf(stderr, "false");
      }
      break;
    }
    OP_DUMPCASE(loadl) {
      fprintf(stderr, "%d", inst->var_id);
      break;
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
  return createInstList(list, inst);
}

InstList convertFLOAT(Node node, InstList list) {
  ScriptCInstruction inst = createInstruction(Idconst);
  inst->double_val = node->double_val;
  return createInstList(list, inst);
}

InstList convertSTRING(Node node, InstList list) {
  ScriptCInstruction inst = createInstruction(Isconst);
  inst->string = node->string;
  return createInstList(list, inst);
}

InstList convertBOOL(Node node, InstList list) {
  ScriptCInstruction inst = createInstruction(Ibconst);
  inst->bool_val = node->bool_val;
  return createInstList(list, inst);
}

InstList convertADD(Node node, InstList list) {
  list = convert(node->child[0], list);
  list = convert(node->child[1], list);
  ScriptCInstruction inst = createInstruction(Iadd);
  list = createInstList(list, inst);
  return list;
}

InstList convertSUB(Node node, InstList list) {
  list = convert(node->child[0], list);
  list = convert(node->child[1], list);
  ScriptCInstruction inst = createInstruction(Isub);
  list = createInstList(list, inst);
  return list;
}

InstList convertMUL(Node node, InstList list) {
  list = convert(node->child[0], list);
  list = convert(node->child[1], list);
  ScriptCInstruction inst = createInstruction(Imul);
  list = createInstList(list, inst);
  return list;
}

InstList convertDIV(Node node, InstList list) {
  list = convert(node->child[0], list);
  list = convert(node->child[1], list);
  ScriptCInstruction inst = createInstruction(Idiv);
  list = createInstList(list, inst);
  return list;
}

InstList convertPLUS(Node node, InstList list) {
  list = convert(node->child[0], list);
  return list;
}

InstList convertMINUS(Node node, InstList list) {
  list = convert(node->child[0], list);
  ScriptCInstruction inst = createInstruction(Iminus);
  list = createInstList(list, inst);
  return list;
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
  VarEntry var = getVarEntry(node->name);
  if(var) {
    ScriptCInstruction inst = createInstruction(Iloadl);
    inst->var_id = var->id;
    list = createInstList(list, inst);
  } else {
    fprintf(stderr, "Error: variable not found (%s)\n", node->name);
    exit(1);
  }
  return list;
}

InstList convertASSIGN(Node node, InstList list) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of assign expression is expected name node\n");
    exit(1);
  }
  list = convert(node->child[1], list);
  ScriptCInstruction inst = createInstruction(Istorel);
  VarEntry var = getVarEntry(node->child[0]->name);
  if(var) {
    inst->var_id = var->id;
  } else {
    inst->var_id = c_context->var_count;
    setVarEntry(node->child[0]->name);
  }
  list = createInstList(list, inst);
  return list;
}

InstList convertSOURCE(Node node, InstList list) {
  if(node->list) {
    ListEntry entry = node->list->elements;
    for(;entry; entry = entry->next) {
      list = convert(entry->node, list);
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
  int size = id;
  ScriptCInstruction* insts = (ScriptCInstruction*)malloc(sizeof(ScriptCInstruction)*size);
  for(int i = 0; i < size; i++) {
    insts[i] = list->inst;
    dumpInstruction(insts[i], i);
    list = list->next;
  }
  return insts;
}

typedef InstList (*convert_to_lir_func_t)(Node, InstList);
static convert_to_lir_func_t f_convert[] = {
#define DEFINE_CONVERT_FUNC(NODE) convert##NODE,
  NODE_EACH(DEFINE_CONVERT_FUNC)
};

static inline InstList convert(Node node, InstList list) {
  return f_convert[node->type](node, list);
}

ScriptCInstruction* compile(Node node) {
  InstList list = createInstList(NULL, createInstruction(Iexit));
  c_context = (CompilerContext) malloc(sizeof(struct CompilerContext));
  c_context->vars = (VarEntry*)malloc(sizeof(VarEntry)*VAR_MAX);
  c_context->var_count = 0;
  f_convert[node->type](node, list);
  return createISeq(list);
}
