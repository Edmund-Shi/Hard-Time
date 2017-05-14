#ifndef SEMANT_H_
#define SEMANT_H_
#include "types.h"
#include "symbol.h"
#include "absyn.h"

/* TODO change the following line after chapter 8 IR code */
typedef void* Tr_exp; /* in order to be easier at the moment */ 

void SEM_transProg(A_exp exp);

// The following functions only available to semant.c file
//struct expty transVar(S_table venv, S_table tenv, A_var v);
//struct expty transExp(S_table venv, S_table tenv, A_exp a);
//void transDec(S_table venv, S_table tenv, A_dec d);
//Ty_ty transTy(S_table tenv, A_ty a);

#endif /* end ifnedf SEMANT_H_ */ 