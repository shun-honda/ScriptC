#ifndef __VM__
#define __VM__

#define IR_EACH(OP)\
	OP(exit)\
	OP(call)\
	OP(ret)\
	OP(iconst)\
  OP(dconst)\
  OP(sconst)\
  OP(bconst)\
	OP(peek)\
	OP(pop)\
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
	OP(plus)\
  OP(minus)\
  OP(loada)\
  OP(loadl)\
  OP(storea)\
  OP(storel)\
  OP(read)\
  OP(write)

enum nezvm_opcode {
#define DEFINE_ENUM(NAME) I##NAME,
  IR_EACH(DEFINE_ENUM)
#undef DEFINE_ENUM
  OP_ERROR = -1
};

union Type {
	int int_val;
	double double_val;
	char* string;
	int bool_val;
};

struct VMContext {
	union Type** var_list;
	union Type** stack_pointer;
	union Type* stack_pointer_base;
	struct VMContext* prev;
}

typedef union Type* Type;
typedef struct VMContext* VMContext;

#endif
