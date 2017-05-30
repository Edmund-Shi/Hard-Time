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
		}var;
		struct {
			Tr_level level;
			Temp_label label;
			Ty_tyList formals;
			Ty_ty result;
		} fun;
	}u;
};

// 初始化构造函数
E_enventry E_VarEntry(Tr_access access, Ty_ty ty);
E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty result);


struct expty SEM_transProg(A_exp exp);

// The following functions only available to semant.c file
//struct expty transVar(S_table venv, S_table tenv, A_var v);
//struct expty transExp(S_table venv, S_table tenv, A_exp a);
//void transDec(S_table venv, S_table tenv, A_dec d);
//Ty_ty transTy(S_table tenv, A_ty a);

#endif /* end ifnedf SEMANT_H_ */ 