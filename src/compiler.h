#ifndef __COMPILER__
#define __COMPILER__

#include "ast.h"

struct VarEntry {
  int id;
  char* name;
};

struct FuncEntry {
  int id;
  int entry_point;
  char* name;
  int arg_size;
};

typedef struct VarEntry* VarEntry;
typedef struct FuncEntry* FuncEntry;

struct ScriptCInstruction {
  int op;
  const void* addr;
  union {
    int int_val;
    double double_val;
    char* string;
    int bool_val;
    int var_id;
    int func_id;
    long call_point;
    int label_id;
    long jump;
  };
};

struct InstList {
  int index;
  struct InstList* prev;
  struct InstList* next;
  struct ScriptCInstruction* inst;
};

#define VAR_MAX 128
#define FUNC_MAX 128
struct CompilerContext {
  int ret;
  struct VarEntry** vars;
  struct FuncEntry** funcs;
  int var_count;
  int func_count;
  long code_length;
  struct CompilerContext* prev;
  struct InstList* root;
  struct InstList* list;
  int* label_list;
  int label_count;
  int id;
  int* breakLabels;
  int* continueLabels;
  int bc_id;
};

#define CC_MAX 128
struct Module {
  int size;
  struct CompilerContext** ctxList;
  long* codePoints;
};

typedef struct CompilerContext* CompilerContext;
typedef struct ScriptCInstruction* ScriptCInstruction;
typedef struct InstList* InstList;
typedef struct Module* Module;

void createModule();
CompilerContext createCompilerContext(CompilerContext prev);
CompilerContext disposeCompilerContext(CompilerContext ctx);
ScriptCInstruction compile(Node node);
void disposeInstruction(ScriptCInstruction inst);

#endif
