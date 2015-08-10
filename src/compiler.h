#ifndef __COMPILER__
#define __COMPILER__

struct ScriptCInstruction {
  int op;
  const void* addr;
  union {
    int int_val;
    double double_val;
    char* string;
    int bool_val;
    int var_id;
    struct ScriptCInstruction* jump;
  };
};

struct InstList {
  int index;
  struct InstList* prev;
  struct InstList* next;
  struct ScriptCInstruction* inst;
};

typedef struct ScriptCInstruction* ScriptCInstruction;
typedef struct InstList* InstList;

ScriptCInstruction* compile(Node node);

#endif
