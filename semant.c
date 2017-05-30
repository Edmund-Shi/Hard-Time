#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "semant.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

// local function dec
struct expty transVar(S_table venv, S_table tenv, A_var v);
struct expty transExp(S_table venv, S_table tenv, A_exp a);
void transDec(S_table venv, S_table tenv, A_dec d);
Ty_ty transTy(S_table tenv, A_ty a);


// IR tree with type check 
struct expty {
	Tr_exp exp;
	Ty_ty ty;
};

struct expty expTy(Tr_exp exp, Ty_ty ty) {
	struct expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

// global dummy node
static struct expty *dummy_expty_p;

void init() {
	dummy_expty_p = (struct expty*) checked_malloc(sizeof(struct expty));
	dummy_expty_p->exp = Tr_voidExp();
	dummy_expty_p->ty = Ty_Void();
}

struct expty SEM_transProg(A_exp exp){
	init();

	// #TODO finish venv and tenv after add local function
	S_table venv = S_empty();
	S_table tenv = S_empty();

	Tr_level mainlevel = Tr_newLevel(Tr_outermost(), Temp_namedlabel("main"), NULL);
	struct expty result = transExp(mainlevel, venv, tenv, exp);
	
	// #TODO print IR tree
	return result;
}
struct expty transExp(Tr_level level,S_table venv, S_table tenv, A_exp a){
	switch (a -> kind){
		case A_intExp:{
			return expTy(a->u.intt, Ty_Int());
		}
		case A_letExp:{
			struct expty exp;
			A_decList d;
			S_beginScope(venv);
			S_beginScope(tenv);
			for(d = a -> u.let.decs; d; d=d->tail){
				transDec(level,venv, tenv, d->head);
			}
			exp = transExp(level,venv,tenv,a->u.let.body);
			S_endScope(tenv);
			S_endScope(venv);
			return exp;
		}
		case A_varExp:{

		}
	
	}

	assert(0);
}
struct expty transVar(Tr_level level, S_table venv, S_table tenv, A_var v){
	switch(v->kind){
		case A_simpleVar:{
			// #TODO add variable
			break;
		}
	}

}

void transDec(Tr_level level, S_table venv, S_table tenv, A_dec d) {
	switch (d->kind){
	case A_varDec: {
		struct expty e = transExp(level,venv, tenv, d->u.var.init);
		// #TODO Unknow function
		// S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
		// The following version is simplified
		S_enter(venv, d->u.var.var, 1); // #bug always return 1
		break;
	}
	case A_typeDec: {
		// #TODO the current version code is only able to 
		// handle single line of type dec(see page 85),so 
		// need further improvements
		S_enter(tenv, d->u.type->head->name,
			transTy(tenv,d->u.type->head->ty));
		break;
	}
	case A_functionDec: {
		// #TODO unimplememt case
		/*A_fundec f = d->u.function->head;
		Ty_ty resultTy = S_look(tenv, f->result);
		*/
		break;
	}
	}
}
Ty_ty transTy(S_table tenv, A_ty a){

}