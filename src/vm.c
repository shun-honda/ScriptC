#include "compiler.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VM_CONTEXT_MAX_STACK_LENGTH 1024

VMContext createVMContext(VMContext prev) {
  VMContext ctx = (VMContext)malloc(sizeof(struct VMContext));
  ctx->var_list_base = (Type)malloc(sizeof(struct Type)*VAR_MAX);
  ctx->var_list = &ctx->var_list_base[0];
  ctx->stack_pointer_base = (Type)malloc(sizeof(struct Type)*VM_CONTEXT_MAX_STACK_LENGTH);
  ctx->stack_pointer = &ctx->stack_pointer_base[0];
  ctx->prev = prev;
  return ctx;
}

VMContext disposeVMContext(VMContext ctx) {
  free(ctx->var_list_base);
  free(ctx->stack_pointer_base);
  VMContext prev = ctx->prev;
  free(ctx);
  return prev;
}

void prepareVM(VMContext ctx, ScriptCInstruction inst, long code_length) {
  const void **table = (const void **)vm_execute(ctx, NULL);
  for(long i = 0; i < code_length; i++) {
    inst->addr = (const void*)table[inst->op];
    ++inst;
  }
}

static inline void push_i(VMContext ctx, int val) {
  (ctx->stack_pointer)->int_val = val;
  (ctx->stack_pointer)->type = TYPE_INT;
  ctx->stack_pointer++;
}

static inline void push_d(VMContext ctx, double val) {
  (ctx->stack_pointer)->double_val = val;
  (ctx->stack_pointer)->type = TYPE_FLOAT;
  ctx->stack_pointer++;
}

static inline void push_s(VMContext ctx, char* val) {
  (ctx->stack_pointer)->string = val;
  (ctx->stack_pointer)->type = TYPE_STRING;
  ctx->stack_pointer++;
}

static inline void push_b(VMContext ctx, int val) {
  (ctx->stack_pointer)->bool_val = val;
  (ctx->stack_pointer)->type = TYPE_BOOL;
  ctx->stack_pointer++;
}

static inline Type pop_sp(VMContext ctx) {
  return --ctx->stack_pointer;
}

#define GET_ADDR(PC) (PC)->addr
#define DISPATCH_NEXT goto *GET_ADDR(++pc)

#define OP(OP) OP_##OP:

long vm_execute(VMContext ctx, ScriptCInstruction inst) {
  static const void *table[] = {
#define DEFINE_TABLE(NAME) &&OP_##NAME,
    IR_EACH(DEFINE_TABLE)
#undef DEFINE_TABLE
  };

  if(inst == NULL) {
    return (long)table;
  }

  register ScriptCInstruction pc = inst+1;

  goto *GET_ADDR(pc);

  OP(exit) {
    return 0;
  }
  OP(call) {

  }
  OP(ret) {

  }
  OP(iconst) {
    push_i(ctx, pc->int_val);
    DISPATCH_NEXT;
  }
  OP(dconst) {
    push_d(ctx, pc->double_val);
    DISPATCH_NEXT;
  }
  OP(sconst) {
    push_s(ctx, pc->string);
    DISPATCH_NEXT;
  }
  OP(bconst) {
    push_b(ctx, pc->bool_val);
    DISPATCH_NEXT;
  }
  OP(peek) {

  }
  OP(pop) {

  }
  OP(jump) {

  }
  OP(ifcmp) {

  }
  OP(gt) {

  }
  OP(ge) {

  }
  OP(lt) {

  }
  OP(le) {

  }
  OP(eq) {

  }
  OP(ne) {

  }
  OP(add) {
    Type right = pop_sp(ctx);
    Type left = pop_sp(ctx);
    if(right->type == TYPE_INT && left->type == TYPE_INT) {
      push_i(ctx, left->int_val + right->int_val);
    } else if(right->type == TYPE_FLOAT && left->type == TYPE_FLOAT) {
      push_d(ctx, left->double_val + right->double_val);
    } else if(right->type == TYPE_STRING && left->type == TYPE_STRING) {
      strcat(left->string, right->string);
      push_s(ctx, left->string);
    } else {
      fprintf(stderr, "type error of add expression\n");
      return 1;
    }
    DISPATCH_NEXT;
  }
  OP(sub) {
    Type right = pop_sp(ctx);
    Type left = pop_sp(ctx);
    if(right->type == TYPE_INT && left->type == TYPE_INT) {
      push_i(ctx, left->int_val - right->int_val);
    } else if(right->type == TYPE_FLOAT && left->type == TYPE_FLOAT) {
      push_d(ctx, left->double_val - right->double_val);
    } else {
      fprintf(stderr, "type error of sub expression\n");
      return 1;
    }
    DISPATCH_NEXT;
  }
  OP(mul) {
    Type right = pop_sp(ctx);
    Type left = pop_sp(ctx);
    if(right->type == TYPE_INT && left->type == TYPE_INT) {
      push_i(ctx, left->int_val * right->int_val);
    } else if(right->type == TYPE_FLOAT && left->type == TYPE_FLOAT) {
      push_d(ctx, left->double_val * right->double_val);
    } else {
      fprintf(stderr, "type error of mul expression\n");
      return 1;
    }
    DISPATCH_NEXT;
  }
  OP(div) {
    Type right = pop_sp(ctx);
    Type left = pop_sp(ctx);
    if(right->type == TYPE_INT && left->type == TYPE_INT) {
      push_i(ctx, left->int_val / right->int_val);
    } else if(right->type == TYPE_FLOAT && left->type == TYPE_FLOAT) {
      push_d(ctx, left->double_val / right->double_val);
    } else {
      fprintf(stderr, "type error of div expression\n");
      return 1;
    }
    DISPATCH_NEXT;
  }
  OP(plus) {

  }
  OP(minus) {

  }
  OP(loada) {

  }
  OP(loadl) {
    Type val = ctx->var_list+pc->var_id;
    if(val->type == TYPE_INT) {
      push_i(ctx, val->int_val);
    } else if(val->type == TYPE_FLOAT) {
      push_d(ctx, val->double_val);
    } else if(val->type == TYPE_STRING) {
      push_s(ctx, val->string);
    } else if(val->type == TYPE_BOOL) {
      push_b(ctx, val->bool_val);
    } else {
      fprintf(stderr, "type error of loadl\n");
      return 1;
    }
    DISPATCH_NEXT;
  }
  OP(storea) {

  }
  OP(storel) {
    Type val = ctx->var_list+pc->var_id;
    Type top = pop_sp(ctx);
    if(top->type == TYPE_INT) {
      val->int_val = top->int_val;
    } else if(top->type == TYPE_FLOAT) {
      val->double_val = top->double_val;
    } else if(top->type == TYPE_STRING) {
      val->string = top->string;
    } else if(top->type == TYPE_BOOL) {
      val->bool_val = top->bool_val;
    } else {
      fprintf(stderr, "type error of storel\n");
      return 1;
    }
    DISPATCH_NEXT;
  }
  OP(read) {

  }
  OP(write) {
    Type val = pop_sp(ctx);
    if(val->type == TYPE_INT) {
      printf("%d", val->int_val);
    } else if(val->type == TYPE_FLOAT) {
      printf("%f", val->double_val);
    } else if(val->type == TYPE_STRING) {
      printf("%s", val->string);
    } else if(val->type == TYPE_BOOL) {
      if(val->bool_val) {
        printf("true");
      } else {
        printf("false");
      }
    }
  }

  return 0;
}