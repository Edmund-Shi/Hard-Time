#include <stdio.h>
#include "translate.h"
#include "tree.h"
#include "frame.h"

static stack_node loop_label_list = NULL;

static void LL_push(Temp_label label) {
	GS_push(&loop_label_list, label);
}
static void LL_pop() {
	GS_pop(&loop_label_list);
}
static Temp_label LL_peek() {
	return GS_peek(&loop_label_list);
}

//functions to translate tree into tr_exp
//only available in c file
static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);
static T_exp unEx(Tr_exp e);
static T_stm unNx(Tr_exp e);
static struct Cx unCx(Tr_exp e);

void doPatch(patchList list, Temp_label label) {
	Temp_label *tmp;
	for (; list != NULL; list = list->tail) {
		tmp = list->head;
		if (tmp != NULL)*tmp = label;
	}
}

static Tr_level outermostLevel = NULL;
Tr_level Tr_outermost() {
	if (outermostLevel == NULL){
		outermostLevel = (Tr_level)checked_malloc(sizeof(struct Tr_level_));
		outermostLevel->parent = NULL;
		outermostLevel->depth = 0;
		// 最外层的一层并不是真正的main，而是包含有预处理信息的
		outermostLevel->frame = F_newFrame(Temp_namedlabel("_main"), NULL);
	}
	return outermostLevel;
}

static Tr_exp Tr_Ex(T_exp ex) {
	Tr_exp p = (Tr_exp)checked_malloc(sizeof *p);
	p->kind = Tr_ex;
	p->u.ex = ex;
	return p;
}
static Tr_exp Tr_Nx(T_stm nx) {
	Tr_exp p = (Tr_exp)checked_malloc(sizeof *p);
	p->kind = Tr_nx;
	p->u.nx = nx;
	return p;
}
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm) {
	Tr_exp p = (Tr_exp)checked_malloc(sizeof *p);
	p->kind = Tr_cx;
	p->u.cx.falses = falses;
	p->u.cx.trues = trues;
	p->u.cx.stm = stm;
	return p;
}
static T_exp unEx(Tr_exp e) {
	switch (e->kind) {
	case Tr_ex:
		return e->u.ex;
	case Tr_cx: {
		Temp_temp r = Temp_newtemp();
		Temp_label t = Temp_newlabel(), f = Temp_newlabel();
		doPatch(e->u.cx.trues, t);
		doPatch(e->u.cx.falses, f);
		return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
			T_Eseq(e->u.cx.stm,
				T_Eseq(T_Label(f),
					T_Eseq(T_Move(T_Temp(r), T_Const(0)),
						T_Eseq(T_Label(t),
							T_Temp(r))))));
	}
	case Tr_nx: {
		return T_Eseq(e->u.nx, T_Const(0));
	}
	//# 错误处理
	//assert(0);
	}
}
static T_stm unNx(Tr_exp e) {
	Temp_label t, f;
	switch (e->kind) {
	case Tr_nx: {
		return e->u.nx;
	}
	case Tr_ex: {
		return T_Exp(e->u.ex);
	}
	case Tr_cx: {
		t = Temp_newlabel();
		f = Temp_newlabel();
		doPatch(e->u.cx.trues, t);
		doPatch(e->u.cx.falses, f);
		return T_Seq(e->u.cx.stm, T_Seq(T_Label(t), T_Label(f)));
	}
	//# 错误处理		
	assert(0);
	}
}
static struct Cx unCx(Tr_exp e) {
	struct Cx cx;
	switch (e->kind) {
	case Tr_ex: {
		cx.stm = T_Cjump(T_ne, e->u.ex, T_Const(0), NULL, NULL);
		cx.trues = PatchList(&cx.stm->u.CJUMP.true, NULL);
		cx.falses = PatchList(&cx.stm->u.CJUMP.false, NULL);
		return cx;
	}
	case Tr_cx: {
		return e->u.cx;
	}
	case Tr_nx: {
		cx.trues = NULL;
		cx.falses = NULL;
		cx.stm = e->u.nx;
		break;
	}
	//# 错误处理
	assert(0);
	}
}
static patchList PatchList(Temp_label *head, patchList tail) {
	patchList p = (patchList)checked_malloc(sizeof(*p));
	p->head = head;
	p->tail = tail;
	return p;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
	Tr_level level = (Tr_level)checked_malloc(sizeof(*level));
	level->parent = parent;
	level->frame = F_newFrame(name, U_BoolList(TRUE, formals));
	level->depth = parent->depth + 1;
	return level;
}
Tr_accessList Tr_formals(Tr_level level) {
	Tr_accessList accList = NULL, accList_head = NULL;
	F_accessList faccList = F_formals(level->frame);

	for (; faccList != NULL; faccList = faccList->tail) {
		if (accList_head == NULL) {
			accList = (Tr_accessList)checked_malloc(sizeof(*accList));
			accList_head = accList;
		}
		else {
			accList->tail = (Tr_accessList)checked_malloc(sizeof(*(accList->tail)));
			accList = accList->tail;
		}
		accList->head = (Tr_access)checked_malloc(sizeof(struct Tr_access_));
		accList->head->level = level;
		accList->head->access = faccList->head;
	}
	if (accList != NULL) {
		accList->tail = NULL;
	}
	return accList_head;
}
Tr_access Tr_allocLocal(Tr_level level, bool escape) {
	F_access acc = F_allocLocal(level->frame, escape);
	Tr_access tracc = (Tr_access)checked_malloc(sizeof(*tracc));
	tracc->access = acc;
	tracc->level = level;
	return tracc;
}


Tr_exp Tr_simpleVar(Tr_access _ac, Tr_level _le) {
	F_access access = _ac->access;
	T_exp exp;
	if (_le != _ac->level) {
		exp = F_Exp(F_staticLink(), T_Temp(F_FP()));
		_le = _le->parent;
		while (_le != _ac->level) {
			exp = F_Exp(F_staticLink(), exp);
			_le = _le->parent;
		}
		exp = F_Exp(access, exp);
	}
	else {
		exp = F_Exp(access, T_Temp(F_FP()));
	}
	return Tr_Ex(exp);
}
Tr_exp Tr_arrayVar(Tr_exp base, Tr_exp offset_exp) {
	//# 应该对数组指针进行下标检查
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(base), T_Binop(T_mul, unEx(offset_exp), T_Const(F_wordSize)))));
}
Tr_exp Tr_ifExp(Tr_exp cond, Tr_exp thenb, Tr_exp elseb) {
	T_exp ex;
	T_stm st;
	Tr_exp ret;
	struct Cx cx_cond = unCx(cond);
	Temp_label t = Temp_newlabel();
	Temp_label f = Temp_newlabel();
	Temp_label join = Temp_newlabel();

	doPatch(cx_cond.trues, t);
	doPatch(cx_cond.falses, f);

	if (elseb == NULL) {
		st = T_Seq(cx_cond.stm,	T_Seq(T_Label(t), T_Seq(unNx(thenb), T_Label(f))));
		ret = Tr_Nx(st);
	}
	else if (thenb->kind == Tr_nx && elseb->kind == Tr_nx) {
		st = T_Seq(cx_cond.stm, T_Seq(T_Label(t), T_Seq(unNx(thenb), T_Seq(T_Jump(T_Name(join), Temp_LabelList(join, NULL)), T_Seq(T_Label(f), T_Seq(unNx(elseb), T_Label(join)))))));
		ret = Tr_Nx(st);
	}
	else {
		Temp_temp r = Temp_newtemp();
		ex = T_Eseq(cx_cond.stm,T_Eseq(T_Label(t),T_Eseq(T_Move(T_Temp(r), unEx(thenb)),T_Eseq(T_Jump(T_Name(join),Temp_LabelList(join, NULL)),T_Eseq(T_Label(f),T_Eseq(T_Move(T_Temp(r), unEx(elseb)),T_Eseq(T_Label(join),T_Temp(r))))))));
		ret = Tr_Ex(ex);
	}
	return ret;
}
Tr_exp Tr_nilExp() {
	return Tr_Ex(T_Mem(T_Const(0)));
}
Tr_exp Tr_assignExp(Tr_exp lvalue, Tr_exp right) {
	if (right != NULL) {
		return Tr_Nx(T_Move(unEx(lvalue), unEx(right)));
	}
	else {
		return lvalue;
	}
}
Tr_exp Tr_intExp(int int_val) {
	return Tr_Ex(T_Const(int_val));
}
Tr_exp Tr_stringExp(string str_val) {
	Temp_label t1 = Temp_newlabel();
	F_String(t1, str_val);
	return Tr_Ex(T_Name(t1));
}
Tr_exp Tr_arithExp(A_oper oper, Tr_exp left, Tr_exp right) {
	switch (oper) {
	case A_plusOp:
		return Tr_Ex(T_Binop(T_plus, unEx(left), unEx(right)));
	case A_minusOp:
		return Tr_Ex(T_Binop(T_minus, unEx(left), unEx(right)));
	case A_timesOp:
		return Tr_Ex(T_Binop(T_mul, unEx(left), unEx(right)));
	case A_divideOp:
		return Tr_Ex(T_Binop(T_div, unEx(left), unEx(right)));
	//# 错误处理
	//assert(0);
	}
	printf("Error in translate.c function Tr_arithExp!");
	exit(1);
}
Tr_exp Tr_voidExp(void) {
	return Tr_Ex(T_Const(0));
}
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init) {
	return Tr_Ex(F_externalCall("initArray", T_ExpList(unEx(size), T_ExpList(unEx(init), NULL))));
}
Tr_exp Tr_recordExp_new(int cnt) {
	T_exp call = F_externalCall("malloc", T_ExpList(T_Const(cnt*F_wordSize), NULL));
	Temp_temp r = Temp_newtemp();
	T_stm move = T_Seq(T_Move(T_Temp(r), call), NULL);
	T_exp ex = T_Eseq(move, T_Temp(r));
	return Tr_Ex(ex);
}
void Tr_recordExp_app(Tr_exp te, Tr_exp init, bool last) {
	T_exp ex = unEx(te);
	T_stm* right = &(ex->u.ESEQ.stm->u.SEQ.right);
	T_exp temp = ex->u.ESEQ.exp;
	int i = 0;
	while (*right != NULL) {
		right = &((*right)->u.SEQ.right);
		i++;
	}
	T_stm move = T_Move(T_Mem(T_Binop(T_plus, temp, T_Const(i*F_wordSize))), unEx(init));
	if (!last) *right = T_Seq(move, NULL);
	else *right = move;
}
Tr_exp Tr_seqExp(Tr_exp* array, int size) {
	T_exp _pex = (T_exp)checked_malloc(sizeof(struct T_exp_));
	T_exp *pex = &_pex;
	T_exp ex_head;
	int i = 0;
	int last = size - 1;
	while (i<size) {
		//Tr_printTrExp(array[i]);
		if (i != last) *pex = T_Eseq(unNx(array[i]), NULL);
		else *pex = unEx(array[i]);
		if (i == 0) ex_head = *pex;
		if (i != last) pex = &((*pex)->u.ESEQ.exp);
		i++;
	}
	return Tr_Ex(ex_head);
}
Tr_exp Tr_logicExp(A_oper oper, Tr_exp left, Tr_exp right, bool isStrCompare) {
	T_stm stm;
	patchList tl;
	patchList fl;
	if (isStrCompare) {
		T_exp call = F_externalCall("stringEqual", T_ExpList(unEx(left), T_ExpList(unEx(right), NULL)));
		switch (oper) {
		case A_eqOp:
			stm = T_Cjump(T_eq, call, T_Const(1), NULL, NULL);
			break;
		case A_neqOp:
			stm = T_Cjump(T_eq, call, T_Const(0), NULL, NULL);
			break;
		//# 错误处理
		//assert(0);
		}
	}
	else {
		switch (oper) {
		case A_eqOp:
			stm = T_Cjump(T_eq, unEx(left), unEx(right), NULL, NULL);
			break;
		case A_neqOp:
			stm = T_Cjump(T_ne, unEx(left), unEx(right), NULL, NULL);
			break;
		case A_ltOp:
			stm = T_Cjump(T_lt, unEx(left), unEx(right), NULL, NULL);
			break;
		case A_gtOp:
			stm = T_Cjump(T_gt, unEx(left), unEx(right), NULL, NULL);
			break;
		case A_leOp:
			stm = T_Cjump(T_le, unEx(left), unEx(right), NULL, NULL);
			break;
		case A_geOp:
			stm = T_Cjump(T_ge, unEx(left), unEx(right), NULL, NULL);
			break;
		//# 错误处理
		//assert(0);
		}
	}
	tl = PatchList(&stm->u.CJUMP.true, NULL);
	fl = PatchList(&stm->u.CJUMP.false, NULL);
	return Tr_Cx(tl, fl, stm);
}
void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals, Temp_label label) {
	T_stm stm;
	stm = T_Seq(T_Label(label), T_Move(T_Temp(F_RV()), unEx(body)));
	F_Proc(stm, level->frame);
}
Tr_exp Tr_callExp(Tr_level caller_lvl, Tr_level callee_lvl, Temp_label fun_label, Tr_exp* argv, int args) {
	int z = 0, cnt = 0;
	T_exp slk;
	if (caller_lvl != callee_lvl) {
		slk = F_Exp(F_staticLink(), T_Temp(F_FP()));
		caller_lvl = caller_lvl->parent;
		while (caller_lvl != callee_lvl) {
			slk = F_Exp(F_staticLink(), slk);
			caller_lvl = caller_lvl->parent;
		}
	}
	else slk = F_Exp(F_staticLink(), T_Temp(F_FP()));
	T_expList listp_head = NULL;
	if (args>0) {
		T_expList listp = (T_expList)checked_malloc(sizeof(struct T_expList_));
		listp_head = listp;
		listp->head = slk;
		listp->tail = NULL;
		int i = 0;
		while (i < args) {
			listp->tail = (T_expList)checked_malloc(sizeof(struct T_expList_));
			listp = listp->tail;
			listp->head = unEx(*argv);
			listp->tail = NULL;
			argv++;
			i++;
		}
	}
	T_exp exp = T_Call(T_Name(fun_label), listp_head);
	return Tr_Ex(exp);
}
Tr_exp Tr_whileExp(Tr_exp cond, Tr_exp body) {
	T_stm st;
	struct Cx cx_cond = unCx(cond);

	Temp_label t = Temp_newlabel();
	Temp_label f = LL_peek();//Get done label from the list, which should have been prepared before this function is called.
	Temp_label start = Temp_newlabel();

	doPatch(cx_cond.trues, t);
	doPatch(cx_cond.falses, f);

	/*
	T_Label  start
	cx_cond.stm (true->t, false->f)
	T_Label(t)
	body
	T_Jump	 start
	T_Label(f)
	*/
	st = T_Seq(T_Label(start),
		T_Seq(cx_cond.stm,
			T_Seq(T_Label(t),
				T_Seq(unNx(body),
					T_Seq(
						T_Jump(
							T_Name(start),
							Temp_LabelList(start, NULL)
							),
						T_Label(f)
						)
					)
				)
			)
		);

	//Pop the label as it's no longer used
	LL_pop();

	return Tr_Nx(st);
}
Tr_exp Tr_forExp(Tr_exp var, Tr_exp low, Tr_exp high, Tr_exp body) {

	T_stm st;
	T_exp v = unEx(var);
	Temp_label t = Temp_newlabel();
	Temp_label f = LL_peek();//Get done label from the list, which should have been prepared before this function is called.
	Temp_label start = Temp_newlabel();

	T_stm cond = T_Cjump(T_le, v, unEx(high), t, f);

	/*
	T_Move   var <- low
	T_Label  start
	T_CJump	 var <= high, t, f
	T_Label(t)
	body
	T_Move   var + 1
	T_Jump	 start
	T_Label(f)
	*/
	st = T_Seq(T_Move(v, unEx(low)),
		T_Seq(T_Label(start),
			T_Seq(cond,
				T_Seq(T_Label(t),
					T_Seq(unNx(body),
						T_Seq(T_Move(v, T_Binop(T_plus, v, T_Const(1))),
							T_Seq(
								T_Jump(
									T_Name(start),
									Temp_LabelList(start, NULL)
									),
								T_Label(f)
								)
							)
						)
					)
				)
			)
		);

	//Pop the label as it's no longer used
	LL_pop();

	return Tr_Nx(st);
}
Tr_exp Tr_breakExp() {
	//Get done label from the list, which should have been prepared before this function is called.
	Temp_label f = LL_peek();
	T_stm st =
		T_Jump(
			T_Name(f),
			Temp_LabelList(f, NULL)
			);

	return Tr_Nx(st);
}
void Tr_genLoopDoneLabel() {
	Temp_label f = Temp_newlabel();
	LL_push(f);
	//return Tr_Ex(T_Name(Temp_newlabel()));
}
Tr_exp Tr_fieldVar(Tr_exp base, int field_offset) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(base), T_Const(field_offset * F_wordSize))));
}

