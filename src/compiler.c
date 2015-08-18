#include "ast.h"
#include "compiler.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static CompilerContext c_context;
static Module module;

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

int containsFunc(char* name) {
  for(int i = 0; i < c_context->func_count; i++) {
    if(!strcmp(name, c_context->funcs[i]->name)) {
      return 1;
    }
  }
  return 0;
}

FuncEntry getFuncEntry(char* name) {
  CompilerContext c_ctx = c_context;
  for(; c_ctx; c_ctx = c_ctx->prev) {
    for(int i = 0; i < c_ctx->func_count; i++) {
      if(!strcmp(name, c_ctx->funcs[i]->name)) {
        return c_ctx->funcs[i];
      }
    }
  }
  return NULL;
}

void setFuncEntry(char* name, int arg_size) {
  c_context->funcs[c_context->func_count] = (FuncEntry)malloc(sizeof(struct FuncEntry));
  c_context->funcs[c_context->func_count]->name = name;
  c_context->funcs[c_context->func_count]->id = module->size;
  c_context->funcs[c_context->func_count]->entry_point = c_context->id;
  c_context->funcs[c_context->func_count]->arg_size = arg_size;
  c_context->func_count++;
  if((c_context->func_count % FUNC_MAX) == 0) {
    c_context->vars = (VarEntry*)realloc(c_context->funcs, sizeof(c_context->funcs)*2);
  }
}

static inline int createLabel() {
  return c_context->label_count++;
}

static inline void setLabel(int id) {
  c_context->label_list[id] = c_context->id;
}

static inline void push_break_continue(int blabel, int clabel) {
  c_context->bc_id++;
  c_context->breakLabels[c_context->bc_id] = blabel;
  c_context->continueLabels[c_context->bc_id] = clabel;
}

static inline int get_break_label() {
  if(c_context->bc_id == -1) {
    fprintf(stderr, "break error\n");
    exit(1);
  }
  return c_context->breakLabels[c_context->bc_id];
}

static inline int get_continue_label() {
  if(c_context->bc_id == -1) {
    fprintf(stderr, "continue error\n");
    exit(1);
  }
  return c_context->continueLabels[c_context->bc_id];
}

static inline void pop_break_continue() {
  c_context->bc_id--;
}

ScriptCInstruction createInstruction(int op) {
  ScriptCInstruction inst = (ScriptCInstruction) malloc(sizeof(struct ScriptCInstruction));
  inst->op = op;
  return inst;
}

InstList createInstList(InstList prev, ScriptCInstruction inst) {
  InstList list = (InstList)malloc(sizeof(struct InstList));
  list->index = c_context->id++;
  list->prev = prev;
  list->next = NULL;
  list->inst = inst;
  if(prev != NULL) {
    prev->next = list;
  }
  return list;
}

void disposeInstList(InstList list) {
  for(; list; list = list->next) {
    free(list->inst);
    free(list);
  }
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
    OP_DUMPCASE(storel) {
      fprintf(stderr, "%d", inst->var_id);
      break;
    }
    OP_DUMPCASE(call) {
      fprintf(stderr, "%ld", inst->call_point);
      break;
    }
    OP_DUMPCASE(jump)
    OP_DUMPCASE(ifcmp) {
      fprintf(stderr, "%ld", inst->jump);
      break;
    }
  default:
    break;
  }
  fprintf(stderr, "\n");
}

static inline void convert(Node node);

void convertNONE(Node node) {
}

void convertINT(Node node) {
  ScriptCInstruction inst = createInstruction(Iiconst);
  inst->int_val = node->int_val;
  c_context->list = createInstList(c_context->list, inst);
}

void convertFLOAT(Node node) {
  ScriptCInstruction inst = createInstruction(Idconst);
  inst->double_val = node->double_val;
  c_context->list = createInstList(c_context->list, inst);
}

void convertSTRING(Node node) {
  ScriptCInstruction inst = createInstruction(Isconst);
  inst->string = node->string;
  c_context->list = createInstList(c_context->list, inst);
}

void convertBOOL(Node node) {
  ScriptCInstruction inst = createInstruction(Ibconst);
  inst->bool_val = node->bool_val;
  c_context->list = createInstList(c_context->list, inst);
}

void convertADD(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Iadd);
  c_context->list = createInstList(c_context->list, inst);
}

void convertSUB(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Isub);
  c_context->list = createInstList(c_context->list, inst);
}

void convertMUL(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Imul);
  c_context->list = createInstList(c_context->list, inst);
}

void convertDIV(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Idiv);
  c_context->list = createInstList(c_context->list, inst);
}

void convertPLUS(Node node) {
  convert(node->child[0]);
}

void convertMINUS(Node node) {
  convert(node->child[0]);
  ScriptCInstruction inst = createInstruction(Iminus);
  c_context->list = createInstList(c_context->list, inst);
}

int countListSize(List list) {
  int count = 0;
  ListEntry entry = list->elements;
  for(; entry; entry = entry->next) {
    count++;
  }
  return count;
}

void convertFUNCDEF(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of assign expression is expected name node\n");
    exit(1);
  }
  if(containsFunc(node->child[0]->name)) {
    fprintf(stderr, "function '%s' is re-defined\n", node->child[0]->name);
    exit(1);
  }
  Node args = node->child[1];
  int count = countListSize(args->list);
  setFuncEntry(node->child[0]->name, count);
  createCompilerContext(c_context);
  ListEntry entry = args->list->elements;
  for(; entry; entry = entry->next) {
    ScriptCInstruction inst = createInstruction(Istorea);
    inst->var_id = c_context->var_count;
    setVarEntry(entry->node->name);
    c_context->list = createInstList(c_context->list, inst);
    if(c_context->root == NULL) {
      c_context->root = c_context->list;
    }
  }
  convert(node->child[2]);
  if(!c_context->ret) {
    ScriptCInstruction inst = createInstruction(Iret_void);
    c_context->list = createInstList(c_context->list, inst);
  }
  c_context = disposeCompilerContext(c_context);
}

void convertARGS(Node node) {
}

void convertSTATEMENTLIST(Node node) {
  ListEntry entry = node->list->elements;
  for(; entry; entry = entry->next) {
    convert(entry->node);
  }
}

void convertNAME(Node node) {
  VarEntry var = getVarEntry(node->name);
  if(var) {
    ScriptCInstruction inst = createInstruction(Iloadl);
    inst->var_id = var->id;
    c_context->list = createInstList(c_context->list, inst);
  } else {
    fprintf(stderr, "Error: variable not found (%s)\n", node->name);
    exit(1);
  }
}

void convertASSIGN(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of assign expression is expected name node\n");
    exit(1);
  }
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Istorel);
  VarEntry var = getVarEntry(node->child[0]->name);
  if(var) {
    inst->var_id = var->id;
  } else {
    inst->var_id = c_context->var_count;
    setVarEntry(node->child[0]->name);
  }
  c_context->list = createInstList(c_context->list, inst);
}

void convertSOURCE(Node node) {
  if(node->list) {
    ListEntry entry = node->list->elements;
    for(;entry; entry = entry->next) {
      convert(entry->node);
    }
  }
}

void convertFUNCCALL(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of function definition is expected name node\n");
    exit(1);
  }
  FuncEntry func = getFuncEntry(node->child[0]->name);
  Node args = node->child[1];
  ListEntry entry = args->list->elements;
  while(entry->next) {
    entry = entry->next;
  }
  for(; entry; entry = entry->prev) {
    convert(entry->node);
  }
  ScriptCInstruction inst = createInstruction(Icall);
  inst->func_id = func->id;
  c_context->list = createInstList(c_context->list, inst);
}

void convertPRINT(Node node) {
  convert(node->child[0]);
  ScriptCInstruction inst = createInstruction(Iwrite);
  c_context->list = createInstList(c_context->list, inst);
}

void convertIF(Node node) {
  int elseLabel = createLabel();
  int mergeLabel = createLabel();
  convert(node->child[0]);
  ScriptCInstruction inst = createInstruction(Iifcmp);
  inst->label_id = elseLabel;
  c_context->list = createInstList(c_context->list, inst);
  convert(node->child[1]);
  inst = createInstruction(Ijump);
  inst->label_id = mergeLabel;
  c_context->list = createInstList(c_context->list, inst);
  setLabel(elseLabel);
  convert(node->child[2]);
  setLabel(mergeLabel);
}

void convertELSE(Node node) {

}

void convertWHILE(Node node) {
  int topLabel = createLabel();
  int endLabel = createLabel();
  push_break_continue(endLabel, topLabel);
  setLabel(topLabel);
  convert(node->child[0]);
  ScriptCInstruction inst = createInstruction(Iifcmp);
  inst->label_id = endLabel;
  c_context->list = createInstList(c_context->list, inst);
  convert(node->child[1]);
  inst = createInstruction(Ijump);
  inst->label_id = topLabel;
  c_context->list = createInstList(c_context->list, inst);
  setLabel(endLabel);
  pop_break_continue();
}

void convertBLOCK(Node node) {
  convert(node->child[0]);
}

void convertRETURN(Node node) {
  convert(node->child[0]);
  ScriptCInstruction inst = createInstruction(Iret);
  c_context->list = createInstList(c_context->list, inst);
  c_context->ret = 1;
}

void convertBREAK(Node node) {
  ScriptCInstruction inst = createInstruction(Ijump);
  inst->label_id = get_break_label();
  c_context->list = createInstList(c_context->list, inst);
}

void convertCONTINUE(Node node) {
  ScriptCInstruction inst = createInstruction(Ijump);
  inst->label_id = get_continue_label();
  c_context->list = createInstList(c_context->list, inst);
}

void convertFOR(Node node) {
  convert(node->child[0]);
  int topLabel = createLabel();
  int endLabel = createLabel();
  int continueLabel = createLabel();
  push_break_continue(endLabel, continueLabel);
  setLabel(topLabel);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Iifcmp);
  inst->label_id = endLabel;
  c_context->list = createInstList(c_context->list, inst);
  convert(node->child[3]);
  setLabel(continueLabel);
  convert(node->child[2]);
  inst = createInstruction(Ijump);
  inst->label_id = topLabel;
  c_context->list = createInstList(c_context->list, inst);
  setLabel(endLabel);
  pop_break_continue();
}

void convertLT(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Ilt);
  c_context->list = createInstList(c_context->list, inst);
}

void convertGT(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Igt);
  c_context->list = createInstList(c_context->list, inst);
}

void convertLE(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Ile);
  c_context->list = createInstList(c_context->list, inst);
}

void convertGE(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Ige);
  c_context->list = createInstList(c_context->list, inst);
}

void convertEQ(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Ieq);
  c_context->list = createInstList(c_context->list, inst);
}

void convertNE(Node node) {
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Ine);
  c_context->list = createInstList(c_context->list, inst);
}

void convertASSIGNADD(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of assign expression is expected name node\n");
    exit(1);
  }
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Iadd);
  c_context->list = createInstList(c_context->list, inst);
  inst = createInstruction(Istorel);
  VarEntry var = getVarEntry(node->child[0]->name);
  if(var) {
    inst->var_id = var->id;
  } else {
    inst->var_id = c_context->var_count;
    setVarEntry(node->child[0]->name);
  }
  c_context->list = createInstList(c_context->list, inst);
}

void convertASSIGNSUB(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of assign expression is expected name node\n");
    exit(1);
  }
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Isub);
  c_context->list = createInstList(c_context->list, inst);
  inst = createInstruction(Istorel);
  VarEntry var = getVarEntry(node->child[0]->name);
  if(var) {
    inst->var_id = var->id;
  } else {
    inst->var_id = c_context->var_count;
    setVarEntry(node->child[0]->name);
  }
  c_context->list = createInstList(c_context->list, inst);
}

void convertASSIGNMUL(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of assign expression is expected name node\n");
    exit(1);
  }
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Imul);
  c_context->list = createInstList(c_context->list, inst);
  inst = createInstruction(Istorel);
  VarEntry var = getVarEntry(node->child[0]->name);
  if(var) {
    inst->var_id = var->id;
  } else {
    inst->var_id = c_context->var_count;
    setVarEntry(node->child[0]->name);
  }
  c_context->list = createInstList(c_context->list, inst);
}

void convertASSIGNDIV(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of assign expression is expected name node\n");
    exit(1);
  }
  convert(node->child[0]);
  convert(node->child[1]);
  ScriptCInstruction inst = createInstruction(Idiv);
  c_context->list = createInstList(c_context->list, inst);
  inst = createInstruction(Istorel);
  VarEntry var = getVarEntry(node->child[0]->name);
  if(var) {
    inst->var_id = var->id;
  } else {
    inst->var_id = c_context->var_count;
    setVarEntry(node->child[0]->name);
  }
  c_context->list = createInstList(c_context->list, inst);
}

void convertINC(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of inc expression is expected name node\n");
    exit(1);
  }
  convert(node->child[0]);
  ScriptCInstruction inst = createInstruction(Iiconst);
  inst->int_val = 1;
  c_context->list = createInstList(c_context->list, inst);
  inst = createInstruction(Iadd);
  c_context->list = createInstList(c_context->list, inst);
  inst = createInstruction(Istorel);
  VarEntry var = getVarEntry(node->child[0]->name);
  if(var) {
    inst->var_id = var->id;
  } else {
    inst->var_id = c_context->var_count;
    setVarEntry(node->child[0]->name);
  }
  c_context->list = createInstList(c_context->list, inst);
}

void convertDEC(Node node) {
  if(node->child[0]->type != SC_NAME) {
    fprintf(stderr, "Error: first argument of dec expression is expected name node\n");
    exit(1);
  }
  convert(node->child[0]);
  ScriptCInstruction inst = createInstruction(Iiconst);
  inst->int_val = 1;
  c_context->list = createInstList(c_context->list, inst);
  inst = createInstruction(Isub);
  c_context->list = createInstList(c_context->list, inst);
  inst = createInstruction(Istorel);
  VarEntry var = getVarEntry(node->child[0]->name);
  if(var) {
    inst->var_id = var->id;
  } else {
    inst->var_id = c_context->var_count;
    setVarEntry(node->child[0]->name);
  }
  c_context->list = createInstList(c_context->list, inst);
}

ScriptCInstruction createISeq(InstList list) {
  int size = 0;
  for(int i = 0; i < module->size; i++) {
    size += module->ctxList[i]->id;
  }
  c_context->code_length = size;
  ScriptCInstruction insts = (ScriptCInstruction)malloc(sizeof(struct ScriptCInstruction)*size);
  ScriptCInstruction root = insts;
  long index = 0;
  for(int i = 0; i < module->size; i++) {
    CompilerContext c_ctx = module->ctxList[i];
    InstList list = c_ctx->root;
    module->codePoints[i] = index;
    for(; list; list = list->next) {
      insts[index] = *list->inst;
      if(insts[index].op == Ijump || insts[index].op == Iifcmp) {
        insts[index].jump = module->codePoints[i] + c_ctx->label_list[insts[index].label_id];
      }
      index++;
    }
    disposeInstList(c_ctx->root);
  }
  for(long i = 0; i < size; i++) {
    if(insts[i].op == Icall) {
      insts[i].call_point = module->codePoints[insts[i].func_id];
    }
    dumpInstruction(&insts[i], i);
  }
  fprintf(stderr, "\n");
  return root;
}

typedef void (*convert_to_lir_func_t)(Node);
static convert_to_lir_func_t f_convert[] = {
#define DEFINE_CONVERT_FUNC(NODE) convert##NODE,
  NODE_EACH(DEFINE_CONVERT_FUNC)
};

static inline void convert(Node node) {
  f_convert[node->type](node);
}

void setCCToModule(CompilerContext cctx);

CompilerContext createCompilerContext(CompilerContext prev) {
  c_context = (CompilerContext)malloc(sizeof(struct CompilerContext));
  c_context->vars = (VarEntry*)malloc(sizeof(VarEntry)*VAR_MAX);
  c_context->funcs = (FuncEntry*)malloc(sizeof(FuncEntry)*FUNC_MAX);
  c_context->label_list = (int*)malloc(sizeof(int)*256);
  c_context->breakLabels = (int*)malloc(sizeof(int)*256);
  c_context->continueLabels = (int*)malloc(sizeof(int)*256);
  c_context->var_count = 0;
  c_context->func_count = 0;
  c_context->prev = prev;
  c_context->ret = 0;
  c_context->label_count = 0;
  c_context->bc_id = -1;
  setCCToModule(c_context);
  return c_context;
}

CompilerContext disposeCompilerContext(CompilerContext ctx) {
  for(int i = 0; i < ctx->var_count; i++) {
    free(ctx->vars[i]);
  }
  free(ctx->vars);
  for(int i = 0; i < ctx->func_count; i++) {
    free(ctx->funcs[i]);
  }
  free(ctx->funcs);
  free(ctx->label_list);
  free(ctx->breakLabels);
  free(ctx->continueLabels);
  CompilerContext prev = ctx->prev;
  free(ctx);
  return prev;
}

void createModule() {
  module = (Module)malloc(sizeof(struct Module));
  module->ctxList = (CompilerContext*)malloc(sizeof(CompilerContext)*CC_MAX);
  module->codePoints = (long*)malloc(sizeof(long)*CC_MAX);
  module->size = 0;
}

void setCCToModule(CompilerContext cctx) {
  module->ctxList[module->size++] = cctx;
  if(module->size % CC_MAX == 0) {
    module->ctxList = (CompilerContext*)realloc(module->ctxList, sizeof(module->ctxList)*2);
    module->codePoints = (long*)realloc(module->codePoints, sizeof(module->codePoints)*2);
  }
}

ScriptCInstruction compile(Node node) {
  c_context->list = createInstList(NULL, createInstruction(Iexit));
  c_context->root = c_context->list;
  f_convert[node->type](node);
  c_context->list = createInstList(c_context->list, createInstruction(Iret_void));
  return createISeq(c_context->root);
}

void disposeInstruction(ScriptCInstruction inst) {
  free(inst);
}
