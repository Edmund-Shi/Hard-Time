#ifndef SEMANT_H_
#define SEMANT_H_
#include "types.h"
#include "symbol.h"
#include "absyn.h"
#include "translate.h"

typedef struct E_enventry_ *E_enventry;
struct E_enventry_ {
	enum { E_varEntry, E_funEntry }kind ;
	union {
		struct {
			Tr_access access;
			Ty_ty ty;
		} var;
		struct {
			Tr_level level;
			Temp_label label;
			Ty_tyList formals;
			Ty_ty result;
		} fun;
	}u;
};
struct expty {
	Tr_exp exp;
	Ty_ty ty;
};
// 初始化构造函数
E_enventry E_VarEntry(Tr_access, Ty_ty);
E_enventry E_FunEntry(Tr_level, Temp_label, Ty_tyList, Ty_ty);

// #bug 可能存在问题
T_stm SEM_transProg(A_exp exp);

#endif /* end ifnedf SEMANT_H_ */ 