#ifndef __VM__
#define __VM__

#define IR_EACH(OP)\
	OP(exit)\
	OP(call)\
	OP(ret)\
	OP(ret_void)\
	OP(iconst)\
  OP(dconst)\
  OP(sconst)\
  OP(bconst)\
	OP(jump)\
	OP(ifcmp)\
	OP(gt)\
	OP(ge)\
	OP(lt)\
	OP(le)\
	OP(eq)\
	OP(ne)\
	OP(add)\
	OP(sub)\
	OP(mul)\
	OP(div)\
  OP(minus)\
  OP(loadl)\
  OP(storea)\
  OP(storel)\
  OP(write)

enum nezvm_opcode {
#define DEFINE_ENUM(NAME) I##NAME,
  IR_EACH(DEFINE_ENUM)
#undef DEFINE_ENUM
  OP_ERROR = -1
};

#define TYPE_INT 0
#define TYPE_FLOAT 1
#define TYPE_STRING 2
#define TYPE_BOOL 3

struct Type {
	int type;
	union {
		int int_val;
		double double_val;
		char* string;
		int bool_val;
	};
};

struct VMContext {
	struct Type* var_list;
	struct Type* var_list_base;
	struct Type* stack_pointer;
	struct Type* stack_pointer_base;
	struct VMContext* prev;
	long retPoint;
};

typedef struct Type* Type;
typedef struct VMContext* VMContext;

VMContext createVMContext(VMContext prev, long retPoint);
void disposeVMContext(VMContext ctx);
void prepareVM(VMContext ctx, ScriptCInstruction inst, long code_length);
long vm_execute(VMContext ctx, ScriptCInstruction inst);

#endif
