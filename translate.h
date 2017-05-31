#ifndef TRANSLATE_H_
#define TRANSLATE_H_
#include "util.h"
#include "tree.h"
#include "temp.h"
#include "frame.h"
#include "absyn.h" // for A_oper

typedef struct patchList_ *patchList;
struct patchList_ {
	Temp_label *head;
	patchList tail;
};
static patchList PatchList(Temp_label *head, patchList tail);

struct Cx {
	patchList trues, falses;
	T_stm stm;
};

typedef struct Tr_exp_ *Tr_exp;
struct Tr_exp_ {
	enum { Tr_ex, Tr_nx, Tr_cx }kind;
	union {
		T_exp ex;
		T_stm nx;
		struct Cx cx;
	} u;
};

typedef struct Tr_access_ *Tr_access;
typedef struct Tr_accessList_ *Tr_accessList;


struct Tr_accessList_ {
	Tr_access head;
	Tr_accessList tail;
};

typedef struct Tr_level_ *Tr_level;
struct Tr_level_ {
	Tr_level parent;
	F_frame frame;
	int depth; // call stack depth
};
struct Tr_access_ {
	Tr_level level;
	F_access access;
};
void Tr_genLoopDoneLabel();
void doPatch(patchList list, Temp_label label);
Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);
Tr_level Tr_getParent(Tr_level parent);

//Tranlation for expressions
Tr_exp Tr_nilExp();

//done
Tr_exp Tr_arithExp(A_oper oper, Tr_exp left, Tr_exp right);
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init);
Tr_exp Tr_arrayVar(Tr_exp base, Tr_exp offset_exp);

Tr_exp Tr_assignExp(Tr_exp lvalue, Tr_exp right);
Tr_exp Tr_callExp(Tr_level caller_lvl, Tr_level callee_lvl, Temp_label fun_label, Tr_exp* argv, int args);
Tr_exp Tr_fieldVar(Tr_exp base, int field_offset);
Tr_exp Tr_ifExp(Tr_exp cond, Tr_exp thenb, Tr_exp elseb);
Tr_exp Tr_intExp(int int_val);
Tr_exp Tr_logicExp(A_oper oper, Tr_exp left, Tr_exp right, bool isStrCompare);
Tr_exp Tr_nilExp();
void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals, Temp_label label);
void Tr_recordExp_app(Tr_exp te, Tr_exp init, bool last);
Tr_exp Tr_recordExp_new(int cnt);
Tr_exp Tr_seqExp(Tr_exp*, int);
Tr_exp Tr_simpleVar(Tr_access, Tr_level);
Tr_exp Tr_stringExp(string str_val);
Tr_exp Tr_voidExp(void);
Tr_exp Tr_whileExp(Tr_exp cond, Tr_exp body);
Tr_exp Tr_breakExp();
Tr_exp Tr_forExp(Tr_exp var, Tr_exp low, Tr_exp high, Tr_exp body);
//Tranlation for expressions
//Tr_exp Tr_forExp(Tr_exp var, Tr_exp low, Tr_exp high, Tr_exp body);



#endif
