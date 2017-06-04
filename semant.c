#include "util.h"
#include "log.h"
#include "symbol.h"
#include "absyn.h"
#include "semant.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include "translate.h"
#include "errormsg.h"

const string TAG = "SEMANT module:";
// 用到的辅助函数定义
void init();
struct expty expTy(Tr_exp, Ty_ty);
S_table E_base_tenv();
S_table E_base_venv();
Ty_ty actual_ty(Ty_ty);
Ty_ty transTyHeader(A_ty);
void createEscapeList(U_boolList, A_fieldList);
static bool pointerCompare(void*, void*);
static void SL_push(S_symbol);
static void SL_pop();
static bool SL_check(S_symbol);
static void SL_empty();
static void VL_push(S_symbol);
static void VL_pop();
static bool VL_check(S_symbol);
static bool VL_isEmpty();
struct expty transExp(Tr_level, S_table, S_table, A_exp);
void transTy(S_table, S_symbol, A_ty);
struct expty transVar(Tr_level, S_table, S_table, A_var);
Tr_exp transDec(Tr_level, S_table, S_table, A_dec);
static bool looseTyCompare(Ty_ty, Ty_ty);
static bool isReferToOutside(bool* , int , S_symbol*, S_symbol);
Ty_tyList makeFormalTyList(S_table, A_fieldList);

//两个初始化构造函数
E_enventry E_VarEntry(Tr_access _access, Ty_ty _ty) {
	E_enventry env = checked_malloc(sizeof(*env));
	env->kind = E_varEntry;
	env->u.var.ty = _ty;
	env->u.var.access = _access;
	return env;
}
E_enventry E_FunEntry(Tr_level _level, Temp_label _label, Ty_tyList _formals, Ty_ty _result) {
	E_enventry env = checked_malloc(sizeof(*env));
	env->kind = E_funEntry;
	env->u.fun.level = _level;
	env->u.fun.label = _label;
	env->u.fun.formals = _formals;
	env->u.fun.result = _result;
	return env;
}
//全局的虚节点头
static struct expty *pDummyExpty;
void init() {
	pDummyExpty = (struct expty*)checked_malloc(sizeof(struct expty));
	pDummyExpty->exp = Tr_voidExp();
	pDummyExpty->ty = Ty_Void();
}
//IR Tree 节点的构造函数
struct expty expTy(Tr_exp _exp, Ty_ty _ty) {
	struct expty e;
	e.exp = _exp;
	e.ty = _ty;
	return e;
}
//基本类型环境初始化
S_table E_base_tenv() {
	S_table table = S_empty();
	S_enter(table, S_Symbol("int"), Ty_Int());
	S_enter(table, S_Symbol("string"), Ty_String());
	return table;
}
//基本值环境初始化，加入默认函数
S_table E_base_venv() {
	S_table table = S_empty();
	Tr_level level;
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("print"), U_BoolList(TRUE, NULL));
	S_enter(table, S_Symbol("print"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_String(), NULL), Ty_Void()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("flush"), NULL);
	S_enter(table, S_Symbol("flush"), E_FunEntry(level, Temp_newlabel(), NULL, Ty_Void()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("getchar"), NULL);
	S_enter(table, S_Symbol("getchar"), E_FunEntry(level, Temp_newlabel(), NULL, Ty_String()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("ord"), U_BoolList(TRUE, NULL));
	S_enter(table, S_Symbol("ord"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_String(), NULL), Ty_Int()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("chr"), U_BoolList(TRUE, NULL));
	S_enter(table, S_Symbol("chr"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_Int(), NULL), Ty_String()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("size"), U_BoolList(TRUE, NULL));
	S_enter(table, S_Symbol("size"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_String(), NULL), Ty_Int()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("substring"), U_BoolList(TRUE, U_BoolList(TRUE, U_BoolList(TRUE, NULL))));
	S_enter(table, S_Symbol("substring"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_String(), Ty_TyList(Ty_Int(), Ty_TyList(Ty_Int(), NULL))), Ty_String()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("concat"), U_BoolList(TRUE, U_BoolList(TRUE, NULL)));
	S_enter(table, S_Symbol("concat"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_String(), Ty_TyList(Ty_String(), NULL)), Ty_String()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("not"), U_BoolList(TRUE, NULL));
	S_enter(table, S_Symbol("not"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_Int(), NULL), Ty_Int()));
	level = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("exit"), U_BoolList(TRUE, NULL));
	S_enter(table, S_Symbol("exit"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_Int(), NULL), Ty_Void()));
	return table;
}
//获得一个类型的真实类型
Ty_ty actual_ty(Ty_ty _ty) {
	if (_ty->kind == Ty_name) return actual_ty(_ty->u.name.ty);
	return _ty;
}
//转换函数
static Ty_ty transTyHeader(A_ty _a) {
	S_symbol sym;
	Ty_ty ty;
	switch (_a->kind) {
	case A_nameTy: return Ty_Name(_a->u.name, NULL);
	case A_recordTy: return Ty_Record(NULL);
	case A_arrayTy:	return Ty_Array(NULL);
	}
}
//创建逃逸变量列表
void createEscapeList(U_boolList *_formals, A_fieldList _fList) {
	A_fieldList list1 = _fList;
	U_boolList list2;
	U_boolList escs = NULL, escs_head = NULL;
	for (; list1 != NULL; list1 = list1->tail) {
		if (escs_head == NULL) {
			escs = U_BoolList(list1->head->escape, NULL);
			escs_head = escs;
		}
		else {
			escs->tail = U_BoolList(list1->head->escape, NULL);
			escs = escs->tail;
		}
	}
	if (escs != NULL) escs->tail = NULL;
	*_formals = escs_head;
}
//堆栈操作，用于循环表达式和 let 表达式，只是将通用的栈接口具体化
static bool pointerCompare(void *_a, void *_b) { return _a == _b ? TRUE : FALSE; }
static stack_node symbol_list = NULL;
static void SL_push(S_symbol _sym) {	GS_push(&symbol_list, _sym); }
static void SL_pop() { GS_pop(&symbol_list); }
static bool SL_check(S_symbol _sym) { return GS_check(symbol_list, _sym, pointerCompare); }
static void SL_empty() { GS_empty(&symbol_list); }
static stack_node variant_list = NULL;
static void VL_push(S_symbol _sym) { GS_push(&variant_list, _sym); }
static void VL_pop() { GS_pop(&variant_list); }
static bool VL_check(S_symbol _sym) { return GS_check(variant_list, _sym, pointerCompare); }
static bool VL_isEmpty() { return variant_list == NULL; }
//转换函数，分各种表达式的情况
struct expty transExp(Tr_level _level, S_table _venv, S_table _tenv, A_exp _exp) {
	string TAG = "TRANSEXP";
	if (_exp == NULL) return *pDummyExpty;
	Tr_exp te; // 总的表达式
	switch (_exp->kind) {
	case A_varExp: return transVar(_level, _venv, _tenv, _exp->u.var);
	case A_stringExp: return expTy(Tr_stringExp(_exp->u.stringg), Ty_String());
	case A_nilExp: return expTy(Tr_nilExp(), Ty_Nil());
	case A_intExp: return expTy(Tr_intExp(_exp->u.intt), Ty_Int());
	case A_callExp: {
		E_enventry entry = S_look(_venv, _exp->u.call.func);
		int arg_number = EXACT_ARGS;
		//查看函数是否定义
		if (entry != NULL && entry->kind == E_funEntry) {
			int i = 0;
			Ty_tyList formals = entry->u.fun.formals;
			while (formals != NULL) {
				if (formals->head != NULL) i++;
				formals = formals->tail;
			}
			Tr_exp* arg_exps = (i > 0) ? (Tr_exp*)checked_malloc(i * sizeof(*arg_exps)) : NULL;
			//检查参数列表
			i = 0;
			A_expList args = _exp->u.call.args;
			for (formals = entry->u.fun.formals; formals != NULL && args != NULL;
			// 遍历所有的节点
			formals = formals->tail, args = args->tail, i++) {
				Ty_ty ty = formals->head;
				A_exp exp = args->head;
				if (exp != NULL && ty != NULL) {
					struct expty expty = transExp(_level, _venv, _tenv, exp);
					if (! looseTyCompare(actual_ty(expty.ty), actual_ty(ty))) EM_error(exp->pos, "Type of argument %d passed to function '%s' is incompatible with the declared type\n", i + 1, S_name(_exp->u.call.func));
					//将参数放到参数链表中
					arg_exps[i] = expty.exp;
				}
				else if (exp == NULL && ty != NULL) {
					arg_number = LESS_ARGS;	break;
				}
				else if (exp != NULL && ty == NULL) {
					arg_number = MORE_ARGS;	break;
				}
			}
			if (formals != NULL && args == NULL) {
				if (formals->head != NULL) arg_number = LESS_ARGS;
			}
			else if (formals == NULL && args != NULL) {
				if (args->head != NULL) arg_number = MORE_ARGS;
			}
			if (arg_number == MORE_ARGS) EM_error(_exp->pos, "More than necessary arguments are passed to function '%s'\n", S_name(_exp->u.call.func));
			else if (arg_number == LESS_ARGS) EM_error(_exp->pos, "Less than necessary arguments are passed to function '%s'\n", S_name(_exp->u.call.func));
			// Generate IR tree
			te = Tr_callExp(_level, Tr_getParent(entry->u.fun.level), entry->u.fun.label, arg_exps, i);
			free(arg_exps);
			return expTy(te, entry->u.fun.result);
		}
		else EM_error(_exp->pos, "Undefined function: %s\n", S_name(_exp->u.call.func));
	}
	case A_opExp: {
		struct expty lexpty, rexpty;
		lexpty = transExp(_level, _venv, _tenv, _exp->u.op.left);
		rexpty = transExp(_level, _venv, _tenv, _exp->u.op.right);
		Ty_ty lty, rty;
		lty = actual_ty(lexpty.ty);
		rty = actual_ty(rexpty.ty);
		A_oper oper = _exp->u.op.oper;
		switch (oper) {
		case A_plusOp:case A_minusOp:case A_timesOp:case A_divideOp:
			// 操作符必须是整型
			if (rty->kind != Ty_int) EM_error(_exp->pos, "The right part of binary operation must be an integer!\n");
			if (lty->kind != Ty_int) EM_error(_exp->pos, "The left part of binary operation must be an integer!\n");
			return expTy(Tr_arithExp(oper, lexpty.exp, rexpty.exp), Ty_Int());
		case A_ltOp:case A_leOp:case A_gtOp:case A_geOp:
			// 操作符也必须是整形
			if (rty->kind != Ty_int) EM_error(_exp->pos, "The right part of binary operation must be an integer!\n");
			if (lty->kind != Ty_int) EM_error(_exp->pos, "The left part of binary operation must be an integer!\n");
			return expTy(Tr_logicExp(oper, lexpty.exp, rexpty.exp, FALSE), Ty_Int());
		case A_eqOp:case A_neqOp:
			// #bug 除了基本类型以外的类型的比较可能会出现bug
			if (rty->kind != lty->kind) EM_error(_exp->pos, "The left part and the right part should be the same type!\n");
			else if (lty->kind == Ty_string) te = Tr_logicExp(oper, lexpty.exp, rexpty.exp, TRUE);
			else te = Tr_logicExp(oper, lexpty.exp, rexpty.exp, FALSE);
			return expTy(te, Ty_Int());
		}
		break;
	}
	case A_recordExp: {
		Ty_ty ty = S_look(_tenv, _exp->u.record.typ);;
		struct expty expty;
		A_exp exp;
		int arg_number = EXACT_ARGS;
		// 检查类型是否一致
		if (ty != NULL && ty->kind == Ty_record) {
			// 计算总数
			int i, cnt;
			Ty_fieldList tyList;
			for (tyList = ty->u.record, cnt = 0; tyList != NULL; tyList = tyList->tail) {
				if (tyList->head != NULL) cnt++;
			}
			te = Tr_recordExp_new(cnt);
			// 递归处理
			A_efield efield;
			Ty_field tyField;
			A_efieldList efList;
			for (tyList = ty->u.record, efList = _exp->u.record.fields, i = 1;
			tyList != NULL && efList != NULL;
				tyList = tyList->tail, efList = efList->tail, i++) {
				tyField = tyList->head;	efield = efList->head;
				if (tyField != NULL && efield != NULL) {
					if (tyField->name != efield->name) EM_error(efield->exp->pos, "The field %d initialized for record '%s' is inconsistent with the declared field's name '%s'\n", i, S_name(_exp->u.record.typ), S_name(tyField->name));
					else {
						// 校验声明
						expty = transExp(_level, _venv, _tenv, efield->exp);
						if (expty.ty != tyField->ty) EM_error(_exp->pos, "Type of field %d initialized for record '%s' is incompatible with the declared type\n", i, S_name(_exp->u.record.typ));
						// 初始化记录值
						Tr_recordExp_app(te, expty.exp, i == cnt ? TRUE : FALSE);//Initialize next field
					}
				}
				else {
					//参数数量发生错误
					if (efield == NULL && tyField != NULL) {
						arg_number = LESS_ARGS;	break;
					}
					else if (efield != NULL && tyField == NULL) {
						arg_number = MORE_ARGS;	break;
					}
				}
			}

			if (arg_number == MORE_ARGS) EM_error(_exp->pos, "More than necessary fields are initialized for record '%s'\n", S_name(_exp->u.record.typ));
			else if (arg_number == LESS_ARGS || (tyList != NULL && tyList->head != NULL)) EM_error(_exp->pos, "Less than necessary fields are initialized for record '%s'\n", S_name(_exp->u.record.typ));
			// 直接返回结果
			return expTy(te, ty);
		}
		else EM_error(_exp->pos, "Undefined type '%s'\n", S_name(_exp->u.record.typ));
		break;
	}
	case A_assignExp: {
		S_symbol var_sym = _exp->u.assign.var->u.simple;
		struct expty lexp, rexp;
		// 检查变量是否在环境中声明过
		// 先计算左边，再计算右边
		lexp = transVar(_level, _venv, _tenv, _exp->u.assign.var);
		rexp = transExp(_level, _venv, _tenv, _exp->u.assign.exp);
		if (actual_ty(lexp.ty) != actual_ty(rexp.ty)) {
			EM_error(_exp->pos, "The left and right type is not the same!\n");
			// 当前只返回一个空的语句，不赋值任何内容
			return expTy(Tr_assignExp(lexp.exp, NULL), Ty_Void());
		}
		return expTy(Tr_assignExp(lexp.exp, rexp.exp), Ty_Void());
		break;
	}
	case A_seqExp: {
		// 计算总共有多少条语句
		int i = 0;
		A_expList expList;
		for (expList = _exp->u.seq; expList != NULL; expList = expList->tail) {
			if (expList->head != NULL) i++;
		}
		// decs
		struct expty resexpty;
		if (i == 0) { resexpty = expTy(Tr_voidExp(), Ty_Void()); }
		else {
			Tr_exp* te_array = (Tr_exp*)checked_malloc(i*sizeof(struct Tr_exp_));
			for (expList = _exp->u.seq, i = 0; expList; expList = expList->tail) {
				// 进行逐语句的翻译
				A_exp exp = expList->head;
				if (exp != NULL) {
					resexpty = transExp(_level, _venv, _tenv, exp);
					te_array[i] = resexpty.exp;
					i++;
				}
			}
			resexpty = expTy(Tr_seqExp(te_array, i), resexpty.ty);
			free(te_array);
		}
		return resexpty;
	}
	case A_ifExp: {
		// 首先翻译测试条件判断语句
		struct expty cond = transExp(_level, _venv, _tenv, _exp->u.iff.test);
		if (actual_ty(cond.ty)->kind != Ty_int) EM_error(_exp->u.iff.test->pos, "The TEST expression is not a number\n");
		// 翻译正确分支
		struct expty trueExp = transExp(_level, _venv, _tenv, _exp->u.iff.then);
		// 翻译错误分支
		struct expty falseExp;
		bool isElse;
		if (_exp->u.iff.elsee != NULL) {
			isElse = TRUE;
			falseExp = transExp(_level, _venv, _tenv, _exp->u.iff.elsee);
		}
		else isElse = FALSE;
		// 测试类型是否一致
		struct expty resexp_ty;
		if (!isElse) {
			if (trueExp.ty->kind != Ty_void) EM_error(_exp->pos, "The if statment should return void!\n");
			resexp_ty = expTy(Tr_ifExp(cond.exp, trueExp.exp, NULL), Ty_Void());
		}
		else {
			if (trueExp.ty->kind != falseExp.ty->kind) EM_error(_exp->pos, "The if...else... statment should return void!\n");
			resexp_ty = expTy(Tr_ifExp(cond.exp, trueExp.exp, falseExp.exp), Ty_Void());
		}
		return resexp_ty;
	}
	case A_whileExp: {
		//利用一个堆栈来实现 break 的跳出，每次进入一个循环就在栈里push一个符号
		VL_push(NULL);
		Tr_genLoopDoneLabel(); // break的时候可以跳转至此处
		struct expty condExp, bodyExp;
		condExp = transExp(_level, _venv, _tenv, _exp->u.whilee.test);
		if (actual_ty(condExp.ty)->kind != Ty_int) EM_error(_exp->pos, "The test exp in WHILE statment must return an INT value.\n");
		bodyExp = transExp(_level, _venv, _tenv, _exp->u.whilee.body);
		if (actual_ty(bodyExp.ty)->kind != Ty_void) EM_error(_exp->pos, "The body of WHILE statment must return void\n");
		// while 语句结束，进行pop
		VL_pop();
		return expTy(Tr_whileExp(condExp.exp, bodyExp.exp), Ty_Void());
	}
	case A_forExp: {
		// 需要注意的是，for语句中可能会定义新的变量，需要新的环境
		S_beginScope(_venv);
		// 检查循环变量是否在外界被使用，如果是，那么就报错，否则压入循环的堆栈
		S_symbol var_sym = _exp->u.forr.var;
		if (VL_check(var_sym)) EM_error(_exp->pos, "The name '%s' has been used in the outer variables\n", S_name(var_sym));
		else VL_push(var_sym);
		Tr_genLoopDoneLabel(); // break的时候可以跳转至此处
							   //计算上限和下限
		struct expty lowExp, highExp;
		lowExp = transExp(_level, _venv, _tenv, _exp->u.forr.lo);
		highExp = transExp(_level, _venv, _tenv, _exp->u.forr.hi);
		if (actual_ty(lowExp.ty)->kind != Ty_int || actual_ty(highExp.ty)->kind != Ty_int)
			EM_error(_exp->pos, "The FOR expression must be integer\n");
		// 进入新的变量环境
		Tr_access tr_acc = Tr_allocLocal(_level, FALSE);
		S_enter(_venv, var_sym, E_VarEntry(tr_acc, Ty_Int()));
		struct expty varExp, bodyExp;
		varExp = transVar(_level, _venv, _tenv, A_SimpleVar(_exp->pos, var_sym));
		bodyExp = transExp(_level, _venv, _tenv, _exp->u.forr.body);
		if (actual_ty(bodyExp.ty)->kind != Ty_void)
			EM_error(_exp->u.forr.body->pos, "Value return form FOR stm should be void\n");
		// 离开环境
		VL_pop();
		S_endScope(_venv);
		return expTy(Tr_forExp(varExp.exp, lowExp.exp, highExp.exp, bodyExp.exp), Ty_Void());
	}
	case A_breakExp: {
		if (VL_isEmpty()) EM_error(_exp->pos, "BREAK only avialiable in loop structure.\n");
		return expTy(Tr_breakExp(), Ty_Void());
	}
	case A_letExp: {
		S_beginScope(_venv); S_beginScope(_tenv);
		int i = 0; // 用来记录一共有多少条定义
		A_decList d;
		for (d = _exp->u.let.decs; d; d = d->tail) {
			if (d->head != NULL) {
				i++;
			}
		}
		Tr_exp * te_array = (Tr_exp *)checked_malloc((i + 1)*sizeof(struct Tr_exp_));
		i = 0;
		// 逐个声明进行遍历
		for (d = _exp->u.let.decs; d; d = d->tail) {
			if (d->head != NULL) {
				te_array[i++] = transDec(_level, _venv, _tenv, d->head);
			}
		}
		struct expty resexpty = transExp(_level, _venv, _tenv, _exp->u.let.body);
		te_array[i] = resexpty.exp;
		resexpty.exp = Tr_seqExp(te_array, i + 1);
		free(te_array);
		S_endScope(_tenv); S_endScope(_venv);
		return resexpty;
	}
	case A_arrayExp: {
		// 首先进行类型检查
		Ty_ty ty = S_look(_tenv, _exp->u.array.typ);
		if (ty != NULL) {
			ty = actual_ty(ty);
			if (ty->kind != Ty_array) EM_error(_exp->pos, "The type need to be an array\n");
		}
		else EM_error(_exp->pos, "Array type doesn't exist\n");
		struct expty resexpty, ele_expty;
		// 检查数组的长度是否是整数
		resexpty = transExp(_level, _venv, _tenv, _exp->u.array.size);
		if (resexpty.ty->kind != Ty_int) EM_error(_exp->pos, "The length of the array must be an integer\n");
		ele_expty = transExp(_level, _venv, _tenv, _exp->u.array.init);
		return expTy(Tr_arrayExp(resexpty.exp, ele_expty.exp), ty);
	}
	}
}
//识别类型的函数
void transTy(S_table _tenv, S_symbol _sym, A_ty a) {
	A_fieldList flist;
	Ty_fieldList tyList = Ty_FieldList(NULL, NULL);
	Ty_fieldList tyList_head = tyList;
	//Get entry of this type from env. 
	Ty_ty this_ty = S_look(_tenv, _sym);
	Ty_ty deref_ty;
	switch (a->kind) {
	case A_nameTy: {
		S_symbol sym = a->u.name;
		Ty_ty ty = S_look(_tenv, sym);
		if (ty == NULL)	EM_error(a->pos, "Undefined type: %s\n", S_name(sym));
		else this_ty->u.name.ty = ty;
		break;
	}
	case A_recordTy: {
		for (flist = a->u.record; flist != NULL; flist = flist->tail) {
			A_field head = flist->head;
			if (head == NULL) continue;
			S_symbol sym = head->typ;
			Ty_ty ty = S_look(_tenv, sym);
			if (ty == NULL)	EM_error(a->pos, "Undefined type: %s\n", S_name(sym));
			else {
				tyList->tail = Ty_FieldList(NULL, NULL);
				tyList->head = Ty_Field(head->name, ty);
				tyList = tyList->tail;
			}
		}
		tyList->tail = NULL;
		this_ty->u.record = tyList_head;
		break;
	}
	case A_arrayTy: {
		S_symbol sym = a->u.array;
		Ty_ty ty = S_look(_tenv, sym);
		if (ty == NULL)	EM_error(a->pos, "Undefined type: %s\n", S_name(sym));
		else this_ty->u.array = ty;
		break;
	}
	}
}
//识别变量的函数
struct expty transVar(Tr_level _level, S_table _venv, S_table _tenv, A_var _var) {
	Tr_exp te;
	switch (_var->kind) {
	case A_simpleVar: {
		E_enventry entry = S_look(_venv, _var->u.simple);
		if (entry != NULL && entry->kind == E_varEntry) {
			te = Tr_simpleVar(entry->u.var.access, _level);
			return expTy(te, actual_ty(entry->u.var.ty));
		}
		else EM_error(_var->pos, "Undefined variable: %s\n", S_name(_var->u.simple));
		break;
	}
	case A_fieldVar: {
		Ty_fieldList fList;
		Ty_field field;
		struct expty exp = transVar(_level, _venv, _tenv, _var->u.field.var);
		if (exp.ty->kind != Ty_record) EM_error(_var->pos, "Field access to a non-record variable\n");
		int offset = 0;
		for (fList = exp.ty->u.record; fList != NULL; fList = fList->tail, offset++) {
			field = fList->head;
			if (field != NULL && field->name == _var->u.field.sym) {
				te = Tr_fieldVar(exp.exp, offset);
				return expTy(te, actual_ty(field->ty));
			}
		}
		EM_error(_var->pos, "Undefined field '%s' in record variable\n", S_name(_var->u.field.sym));
		break;
	}
	case A_subscriptVar: {
		// 数组的遍历
		struct expty lexp = transVar(_level, _venv, _tenv, _var->u.subscript.var);
		if (lexp.ty->kind != Ty_array) {
			EM_error(_var->pos, "Indexed access to a non-array variable\n");
			break;
		}
		// 检查整数
		struct expty rexp = transExp(_level, _venv, _tenv, _var->u.subscript.exp);
		if (rexp.ty->kind != Ty_int) {
			EM_error(_var->pos, "Indexed access to array variable must be of integer type\n");
			break;
		}
		te = Tr_arrayVar(lexp.exp, rexp.exp);
		// 获取真实的类型
		return expTy(te, actual_ty(lexp.ty->u.array));
	}

	}

}
//识别定义的函数
Tr_exp transDec(Tr_level _level, S_table _venv, S_table _tenv, A_dec _dec) {
	Tr_exp tr_exp;
	if (_dec == NULL) return Tr_voidExp();
	A_nametyList types;
	A_namety type;
	A_fundecList functions;
	A_fundec head;
	struct expty e;
	Ty_ty varDecTy, resultTy, ty;
	Ty_tyList formalTys, tList;
	E_enventry entry;
	A_fieldList fList;
	/* used for frame manipulation */
	Tr_access tr_acc;
	U_boolList formal_escs;
	Tr_level funLevel;
	Tr_accessList tr_formals;
	/* used in detecting cyclic type definition */
	A_ty* nameTys, aty, refered;
	int names = 0, ind, total_marks, marked_len;
	bool changed, *marked;
	S_symbol* tySymbols;
	switch (_dec->kind) {
	case A_varDec: {
		S_symbol var_sym = _dec->u.var.var;
		//翻译初始化语句
		e = transExp(_level, _venv, _tenv, _dec->u.var.init);
		ty = actual_ty(e.ty);
		// 检测变量类型
		if (_dec->u.var.typ != NULL) {
			varDecTy = S_look(_tenv, _dec->u.var.typ);
			if (varDecTy == NULL) EM_error(_dec->pos, "Variable type incompatible!\n");
		}
		else {
			if (ty->kind == Ty_nil) EM_error(_dec->pos, "NIL can't be variable type\n");
			varDecTy = e.ty;
		}
		tr_acc = Tr_allocLocal(_level, _dec->u.var.escape);//always escaping
		S_enter(_venv, var_sym, E_VarEntry(tr_acc, varDecTy));
		tr_exp = Tr_assignExp(transVar(_level, _venv, _tenv, A_SimpleVar(_dec->pos, _dec->u.var.var)).exp, e.exp);
		break;
	}
	case A_typeDec: {
		SL_empty();
		for (types = _dec->u.type; types != NULL; types = types->tail) {
			type = types->head;
			if (type != NULL) {
				// 是否被引用到
				if (SL_check(type->name)) EM_error(_dec->pos, "The type name '%s' has been used adjacently\n", S_name(type->name));
				else SL_push(type->name);
				S_enter(_tenv, type->name, transTyHeader(type->ty));
				if (type->ty->kind == A_nameTy) names++;
			}
		}
		// 防止被重复循环定义
		nameTys = checked_malloc(sizeof(A_ty)*names);
		tySymbols = checked_malloc(sizeof(S_symbol)*names);
		marked = checked_malloc(sizeof(bool)*names);
		for (ind = 0; ind < names; ind++) marked[ind] = FALSE;
		marked_len = names;
		names = 0;
		for (types = _dec->u.type; types != NULL; types = types->tail) {
			type = types->head;
			if (type != NULL) {
				transTy(_tenv, type->name, type->ty);
				if (type->ty->kind == A_nameTy) {
					tySymbols[names] = type->name;
					nameTys[names++] = type->ty;
				}
			}
		}
		// 检查是否发生循环定义
		total_marks = names;
		while (total_marks>0) {
			changed = FALSE;
			for (ind = 0; ind < names; ind++) {
				if (marked[ind] == TRUE) continue;
				aty = nameTys[ind];
				if (isReferToOutside(marked, marked_len, tySymbols, aty->u.name)) {
					marked[ind] = TRUE;
					changed = TRUE;
					total_marks--;
				}
			}
			if (changed == FALSE && total_marks>0) {
				for (ind = 0; ind < names; ind++)
					if (marked[ind] == FALSE) {
						aty = nameTys[ind];
						EM_error(aty->pos, "Cyclic type definition detected: %s\n", S_name(tySymbols[ind]));
					}
				break;
			}
		}
		free(nameTys);
		free(tySymbols);
		free(marked);
		tr_exp = Tr_voidExp();
		break;
	}
	case A_functionDec: {
		SL_empty();
		//检查逃逸变量
		for (functions = _dec->u.function; functions != NULL; functions = functions->tail) {
			head = functions->head;
			if (head != NULL) {
				// 检查重定义
				if (SL_check(head->name)) EM_error(_dec->pos, "The function name '%s' has been used adjacently\n", S_name(head->name));
				else SL_push(head->name);
				// 检查返回值
				if (head->result != NULL) resultTy = S_look(_tenv, head->result);
				else resultTy = Ty_Void();
				formalTys = makeFormalTyList(_tenv, head->params);
				//新的一层
				createEscapeList(&formal_escs, head->params);
				funLevel = Tr_newLevel(_level, Temp_namedlabel(S_name(head->name)), formal_escs);
				free(formal_escs);
				S_enter(_venv, head->name, E_FunEntry(funLevel, Temp_newlabel(), formalTys, resultTy));
			}
		}
		//2) 函数的声明
		for (functions = _dec->u.function; functions != NULL; functions = functions->tail) {
			head = functions->head;
			if (head != NULL) {
				entry = S_look(_venv, head->name);
				formalTys = entry->u.fun.formals;
				// 添加变量到新的环境中
				S_beginScope(_venv);
				tr_formals = Tr_formals(entry->u.fun.level)->tail;
				for (fList = head->params, tList = formalTys; fList != NULL;
				fList = fList->tail, tList = tList->tail, tr_formals = tr_formals->tail) {
					S_enter(_venv, fList->head->name, E_VarEntry(tr_formals->head, tList->head));
				}
				// 翻译函数体
				e = transExp(entry->u.fun.level, _venv, _tenv, head->body);
				// 进行类型检查
				resultTy = entry->u.fun.result;
				if (head->result != NULL) {
					ty = actual_ty(e.ty);
					if (actual_ty(resultTy) != ty) {
						if (ty->kind == Ty_void) EM_error(head->pos, "The function '%s' has no value returned\n", S_name(head->name));
						else EM_error(head->pos, "The returned value of function '%s' is incompatible with the declared one\n", S_name(head->name));
					}
				}
				else if (actual_ty(e.ty)->kind != Ty_void) EM_error(head->pos, "The returned value of procedure '%s' will be ignored\n", S_name(head->name));
				S_endScope(_venv);
			}
		}
		tr_exp = Tr_voidExp();
		break;
	}
	}
	return tr_exp;
}
//判断两个类型是不是相同
static bool looseTyCompare(Ty_ty _a, Ty_ty _b) {
	Ty_ty a = actual_ty(_a);
	Ty_ty b = actual_ty(_b);
	if (a == b) return TRUE;
	if (a->kind == Ty_nil || b->kind == Ty_nil) return TRUE;
	if (a->kind == Ty_string && b->kind == Ty_int){
		return TRUE;
	}
	if (a->kind == Ty_int && b->kind == Ty_string){
		return TRUE;
	}
	return FALSE;
}
//检查是否在外部定义
static bool isReferToOutside(bool* _marked, int _marked_len, S_symbol* _syms, S_symbol _refered) {
	int i;
	for (i = 0; i < _marked_len; i++) {
		if (_syms[i] == _refered) return _marked[i];
	}
	return TRUE;
}
//制作类型链表
Ty_tyList makeFormalTyList(S_table _tenv, A_fieldList _params) {
	Ty_tyList tyList = Ty_TyList(NULL, NULL);
	Ty_tyList tyList_head = tyList;
	bool legal = TRUE;
	S_symbol _sym;
	Ty_ty ty;
	SL_empty();
	for (; _params != NULL; _params = _params->tail) {
		A_field head = _params->head;
		if (head == NULL) continue;
		if (VL_check(head->name)) EM_error(head->pos, "The name of loop variant '%s' cannot be reused by function's argument\n", S_name(head->name));
		// 检查是否被定义
		_sym = head->typ;
		ty = S_look(_tenv, _sym);
		if (ty == NULL) {
			EM_error(head->pos, "Undefined type: %s\n", S_name(_sym));
			legal = legal && FALSE;
		}
		// 名字没有出现过的话就要插入一个新的名字?
		if (!SL_check(head->name)) { SL_push(head->name); }
		else EM_error(head->pos, "The name of formal argument '%s' has been used for this function\n", S_name(head->name));
		if (legal) {
			tyList->tail = Ty_TyList(NULL, NULL);
			tyList->head = ty;
			tyList = tyList->tail;
		}
	}
	tyList->tail = NULL;
	return tyList_head->head != NULL ? tyList_head : NULL;
}
//接口函数，用于外部调用，得到 IR Tree
T_stm SEM_transProg(A_exp _exp) {
	init();
	S_table venv = E_base_venv(), tenv = E_base_tenv();
	Tr_level mainlevel = Tr_newLevel(Tr_outLevel(), Temp_namedlabel("main"), NULL);
	struct expty result = transExp(mainlevel, venv, tenv, _exp);
	return getResult(result);
}

