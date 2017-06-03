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
// local function dec
struct expty transVar(Tr_level level, S_table venv, S_table tenv, A_var v);
struct expty transExp(Tr_level level,S_table venv, S_table tenv, A_exp a);
Tr_exp transDec(Tr_level level, S_table venv, S_table tenv, A_dec d);
Ty_ty transTy(S_table tenv, S_symbol sym, A_ty a);
Ty_ty actual_ty(Ty_ty ty); // return the actual type of ty
Ty_ty transTyHeader(A_ty a);
bool isReferToOutside(bool* marked, int marked_len, S_symbol* syms, S_symbol refered);
Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params);
void createEscapeList(U_boolList *formal_escs, A_fieldList fList);

string TAG = "SEMANT module:";

// function for stack operations
/* Loop variants START */
static stack_node variant_list = NULL;//variant_node 

static bool pointerCompare(void *a, void *b) {
	return a == b ? TRUE : FALSE;
}

/* Symbol list START */
static void SL_push(S_symbol sym);
static void SL_pop();
static bool SL_check(S_symbol sym);
static void SL_empty();

static stack_node symbol_list = NULL;

static void SL_push(S_symbol sym) {
	GS_push(&symbol_list, sym);
}
static void SL_pop() {
	GS_pop(&symbol_list);
}
static bool SL_check(S_symbol sym) {
	return GS_check(symbol_list, sym, pointerCompare);
}
static void SL_empty() {
	GS_empty(&symbol_list);
}
/* Symbol list END */

static void VL_push(S_symbol sym) {
	GS_push(&variant_list, sym);
}
static void VL_pop() {
	GS_pop(&variant_list);
}
static bool VL_check(S_symbol sym) {
	return GS_check(variant_list, sym, pointerCompare);
}
static bool VL_isEmpty() {
	return variant_list == NULL;
}


S_table E_base_tenv();
S_table E_base_venv();

// IR tree with type check 

struct expty expTy(Tr_exp exp, Ty_ty ty) {
	struct expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

// 全局的虚结点头
static struct expty *dummy_expty_p;

void init() {
	dummy_expty_p = (struct expty*) checked_malloc(sizeof(struct expty));
	dummy_expty_p->exp = Tr_voidExp();
	dummy_expty_p->ty = Ty_Void();
}

Tr_level Tr_getParent(Tr_level level) {
	return level->parent;
}

T_stm SEM_transProg(A_exp exp){
	init();

	S_table venv = E_base_venv();
	S_table tenv = E_base_tenv();

	Tr_level mainlevel = Tr_newLevel(Tr_outermost(), Temp_namedlabel("main"), NULL);
	struct expty result = transExp(mainlevel, venv, tenv, exp);
	
	// #TODO print IR tree
	return getResult(result);
	/*if (result.exp->kind == Tr_ex){
		return result.exp->u.ex;
	}else if (result.exp->kind == Tr_nx){
		return result.exp->u.nx;
	}
	else {
		Log("SEMANT","ERROR: CX find in the final result");
	}*/
}
#define EXACT_ARGS 0
#define LESS_ARGS 1
#define MORE_ARGS 2

struct expty transVar(Tr_level level, S_table venv, S_table tenv, A_var v){
	E_enventry entry;

	Ty_fieldList fList;
	Ty_field field;
	struct expty exp;
	struct expty exp2;

	Tr_exp te;
	int offset;


	switch(v->kind){
		case A_simpleVar:{
			entry = S_look(venv, v->u.simple);
			if (entry != NULL && entry->kind == E_varEntry) {
				te = Tr_simpleVar(entry->u.var.access, level);
				return expTy(te, actual_ty(entry->u.var.ty));
			}
			else {
				EM_error( v->pos, "Undefined variable: %s", S_name(v->u.simple));
				break;
			}
			break;
		}
		case A_fieldVar: {
			exp = transVar(level, venv, tenv, v->u.field.var);
			if (exp.ty->kind != Ty_record) {
				EM_error(v->pos, "Field access to a non-record variable");
			}
			offset = 0;
			for (fList = exp.ty->u.record; fList != NULL; fList = fList->tail, offset++) {
				field = fList->head;
				if (field != NULL && field->name == v->u.field.sym) {
					te = Tr_fieldVar(exp.exp, offset);
					return expTy(te, actual_ty(field->ty));
				}
			}
			EM_error(v->pos, "Undefined field '%s' in record variable", S_name(v->u.field.sym));
			break;
		}
		case A_subscriptVar: {
			// 数组的遍历
			exp = transVar(level, venv, tenv, v->u.subscript.var);
			if (exp.ty->kind != Ty_array) {
				EM_error(v->pos, "Indexed access to a non-array variable");
				break;
			}

			// 检查整数
			exp2 = transExp(level, venv, tenv, v->u.subscript.exp);
			if (exp2.ty->kind != Ty_int) {
				EM_error( v->pos, "Indexed access to array variable must be of integer type");
				break;
			}
			te = Tr_arrayVar(exp.exp, exp2.exp);

			// 获取真实的类型
			return expTy(te, actual_ty(exp.ty->u.array));
		}

	}

}
Ty_ty transTy(S_table tenv, S_symbol sym, A_ty a) {
	A_fieldList flist;
	Ty_fieldList tyList = Ty_FieldList(NULL, NULL);
	Ty_fieldList tyList_head = tyList;

	//Get entry of this type from env. 
	Ty_ty this_ty = S_look(tenv, sym);
	Ty_ty deref_ty;

	S_symbol _sym;
	Ty_ty ty;


	switch (a->kind) {//A_nameTy, A_recordTy, A_arrayTy
	case A_nameTy:
		_sym = a->u.name;
		ty = S_look(tenv, _sym);
		if (ty == NULL) {
			EM_error(a->pos, "Undefined type: %s", S_name(_sym));
		}
		else {
			this_ty->u.name.ty = ty;
		}
		break;
	case A_recordTy:
		for (flist = a->u.record; flist != NULL; flist = flist->tail) {
			A_field head = flist->head;
			if (head == NULL) {
				continue;
			}
			_sym = head->typ;
			ty = S_look(tenv, _sym);
			if (ty == NULL) {
				EM_error(a->pos, "Undefined type: %s", S_name(_sym));
			}
			else {
				tyList->tail = Ty_FieldList(NULL, NULL);
				tyList->head = Ty_Field(head->name, ty);
				tyList = tyList->tail;
			}
		}

		tyList->tail = NULL;
		this_ty->u.record = tyList_head;
		break;
	case A_arrayTy:
		_sym = a->u.array;
		ty = S_look(tenv, _sym);
		if (ty == NULL) {
			EM_error(a->pos, "Undefined type: %s", S_name(_sym));
		}
		else {
			this_ty->u.array = ty;
		}
		break;
	}
}

Tr_exp transDec(Tr_level level, S_table venv, S_table tenv, A_dec d) {
	Tr_exp tr_exp;
	if (d == NULL){
		// 如果是空，返回void
		tr_exp = Tr_voidExp();
		return tr_exp;
	}
	A_nametyList types;
	A_namety type;
	A_fundecList functions;
	A_fundec head;
	struct expty e;
	Ty_ty varDecTy;
	Ty_ty resultTy;
	Ty_ty ty;
	Ty_tyList formalTys;
	E_enventry entry;
	A_fieldList fList;
	Ty_tyList tList;

	/* used for frame manipulation */
	Tr_access tr_acc;
	U_boolList formal_escs;
	Tr_level funLevel;
	Tr_accessList tr_formals;

	/* used in detecting cyclic type definition */
	A_ty* nameTys;
	int names = 0;
	bool changed;
	int ind;
	A_ty aty;
	bool* marked;
	A_ty refered;
	S_symbol* tySymbols;
	S_symbol var_sym;
	int total_marks;
	int marked_len;

	switch (d->kind){
	case A_varDec: {
		var_sym = d->u.var.var;
		
		// translate the initializer
		e = transExp(level,venv, tenv, d->u.var.init);
		ty = actual_ty(e.ty);

		// 检测变量类型
		if (d->u.var.typ != NULL){
			varDecTy = S_look(tenv, d->u.var.typ);
			if (varDecTy == NULL){
				EM_error(d->pos, "Variable type incompatible!");
			}
		}
		else {
			if (ty->kind == Ty_nil) { 
				EM_error(d->pos, "NIL can't be variable type");
			}
			varDecTy = e.ty;
		}
	
		tr_acc = Tr_allocLocal(level, TRUE);//always escaping
		S_enter(venv, var_sym, E_VarEntry(tr_acc, varDecTy));
		tr_exp = Tr_assignExp(transVar(level, venv, tenv, A_SimpleVar(d->pos, d->u.var.var)).exp, e.exp);
		break;
	}
	case A_typeDec: {
		SL_empty();
		for (types = d->u.type; types != NULL; types = types->tail) {
			type = types->head;
			if (type != NULL) {
				// 是否被引用到
				if (SL_check(type->name)) {
					EM_error(d->pos,
						"The type name '%s' has been used adjacently",
						S_name(type->name));
				}
				else {
					SL_push(type->name);
				}

				S_enter(tenv, type->name, transTyHeader(type->ty));
				if (type->ty->kind == A_nameTy) {
					names++;
				}
			}
		}

		// 防止被重复循环定义
		nameTys = checked_malloc(sizeof(A_ty)*names);
		tySymbols = checked_malloc(sizeof(S_symbol)*names);
		marked = checked_malloc(sizeof(bool)*names);
		for (ind = 0; ind < names; ind++) {
			marked[ind] = FALSE;
		}
		marked_len = names;
		names = 0;

		for (types = d->u.type; types != NULL; types = types->tail) {
			type = types->head;
			if (type != NULL) {
				transTy(tenv, type->name, type->ty);
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
				if (marked[ind] == TRUE) {
					continue;
				}
				aty = nameTys[ind];
				if (isReferToOutside(marked, marked_len, tySymbols, aty->u.name)) {
					marked[ind] = TRUE;
					changed = TRUE;
					total_marks--;
				}
			}
			if (changed == FALSE && total_marks>0) {
				for (ind = 0; ind < names; ind++) {
					if (marked[ind] == FALSE) {
						aty = nameTys[ind];
						EM_error(aty->pos, "Cyclic type definition detected: %s", S_name(tySymbols[ind]));
					}
				}
				
				break;
			}
		}
		free(nameTys);
		free(tySymbols);
		free(marked);

		tr_exp = Tr_voidExp();//Return NO_OP
		break;
	}
	
	case A_functionDec: {
		SL_empty();
		for (functions = d->u.function; functions != NULL; functions = functions->tail) {
			head = functions->head;
			if (head != NULL) {

				// 检查重定义
				if (SL_check(head->name)) {
					EM_error(d->pos,
						"The function name '%s' has been used adjacently",
						S_name(head->name));
				}
				else {
					SL_push(head->name);
				}

				// 检查返回值
				if (head->result != NULL) {
					resultTy = S_look(tenv, head->result);
				}
				else {
					resultTy = Ty_Void();
				}
 
				formalTys = makeFormalTyList(tenv, head->params);

				//新的一层
				createEscapeList(&formal_escs, head->params);
				funLevel = Tr_newLevel(level, Temp_namedlabel(S_name(head->name)), formal_escs);
				free(formal_escs);
				S_enter(venv, head->name, E_FunEntry(funLevel, Temp_newlabel(), formalTys, resultTy));
			}
		}

		//2) 函数的声明
		for (functions = d->u.function; functions != NULL; functions = functions->tail) {
			head = functions->head;
			if (head != NULL) {
				
				entry = S_look(venv, head->name);
				formalTys = entry->u.fun.formals;

				// 添加变量到新的环境中
				S_beginScope(venv);
				tr_formals = Tr_formals(entry->u.fun.level)->tail;
				for (fList = head->params, tList = formalTys; fList != NULL;
				fList = fList->tail, tList = tList->tail, tr_formals = tr_formals->tail) {
					S_enter(venv, fList->head->name, E_VarEntry(tr_formals->head, tList->head));
				}
				// translate the body
				e = transExp(entry->u.fun.level, venv, tenv, head->body);

				//check the type compatibility 
				resultTy = entry->u.fun.result;
				if (head->result != NULL) {
					//2.5.1) it's a function
					ty = actual_ty(e.ty);
					if (actual_ty(resultTy) != ty) {
						if (ty->kind == Ty_void) {
							EM_error(head->pos,
								"The function '%s' has no value returned",
								S_name(head->name));
						}
						else {
							EM_error(head->pos,
								"The returned value of function '%s' is incompatible with the declared one",
								S_name(head->name));
						}
					}
				}
				else {
					if (actual_ty(e.ty)->kind != Ty_void) {
						EM_error(head->pos,
							"The returned value of procedure '%s' will be ignored",
							S_name(head->name));
					}
				}
				S_endScope(venv);
			}
		}

		tr_exp = Tr_voidExp();//Return NO_OP
		break;
	}	
	}
	return tr_exp;
}

struct expty transExp(Tr_level level, S_table venv, S_table tenv, A_exp a) {
	string TAG = "TRANSEXP";
	if (a == NULL) {
		return *dummy_expty_p;
	}

	Tr_exp te; // 总的表达式
	switch (a->kind) {
	case A_varExp: {
		return transVar(level, venv, tenv, a->u.var);
	}
	case A_stringExp: {
		te = Tr_stringExp(a->u.stringg);
		return expTy(te, Ty_String());
	}
	case A_nilExp: {
		te = Tr_nilExp();
		return expTy(te, Ty_Nil());
	}
	case A_intExp: {
		te = Tr_intExp(a->u.intt);
		return expTy(te, Ty_Int());
	}
	case A_callExp: {
		Ty_tyList formals;
		A_expList args;
		Tr_exp* arg_exps;
		struct expty expty;
		Ty_ty ty;
		E_enventry entry;
		int arg_number;
		arg_number = EXACT_ARGS;
		A_exp exp;
		entry = S_look(venv, a->u.call.func);
		// check if function is defined
		int i;
		if (entry != NULL && entry->kind == E_funEntry) {
			i = 0;
			formals = entry->u.fun.formals;
			while (formals != NULL) {
				if (formals->head != NULL) {
					i++;
				}
				formals = formals->tail;
			}
			arg_exps = (i > 0) ? (Tr_exp*)checked_malloc(i * sizeof(Tr_exp)) : NULL;
			// Check the arguments
			i = 0;
			for (formals = entry->u.fun.formals, args = a->u.call.args;
			formals != NULL && args != NULL;
				// 遍历所有的节点
				formals = formals->tail, args = args->tail, i++) {
				ty = formals->head;
				exp = args->head;
				if (exp != NULL && ty != NULL) {
					expty = transExp(level, venv, tenv, exp);
					if (actual_ty(expty.ty) != actual_ty(ty)) {
						EM_error(exp->pos,
							"Type of argument %d passed to function '%s' is incompatible with the declared type",
							i + 1, S_name(a->u.call.func));
					}
					// Put argument into the arg list
					arg_exps[i] = expty.exp;
				}
				else {
					if (exp == NULL && ty != NULL) {
						arg_number = LESS_ARGS;
						break;
					}
					else if (exp != NULL && ty == NULL) {
						arg_number = MORE_ARGS;
						break;
					}
				}
			}
			if (formals != NULL && args == NULL) {
				if (formals->head != NULL) {
					arg_number = LESS_ARGS;
				}
			}
			else if (formals == NULL && args != NULL) {
				if (args->head != NULL) {
					arg_number = MORE_ARGS;
				}
			}
			if (arg_number == MORE_ARGS) {
				EM_error(a->pos,
					"More than necessary arguments are passed to function '%s'", S_name(a->u.call.func));
			}
			else if (arg_number == LESS_ARGS) {
				EM_error(a->pos,
					"Less than necessary arguments are passed to function '%s'", S_name(a->u.call.func));
			}

			// Generate IR tree
			te = Tr_callExp(level, Tr_getParent(entry->u.fun.level), entry->u.fun.label, arg_exps, i);
			free(arg_exps);

			return expTy(te, entry->u.fun.result);
		}
		else {
			EM_error(a->pos, "Undefined function: %s", S_name(a->u.call.func));

		}
	}
	case A_opExp: {
		struct expty left_expty, right_expty;
		left_expty = transExp(level, venv, tenv, a->u.op.left);
		right_expty = transExp(level, venv, tenv, a->u.op.right);
		Ty_ty left_ty, right_ty;
		left_ty = actual_ty(left_expty.ty);
		right_ty = actual_ty(right_expty.ty);
		A_oper oper = a->u.op.oper;

		switch (oper) {
		case A_plusOp:case A_minusOp:case A_timesOp:case A_divideOp:
			// left and right part must integers
			if (right_ty->kind != Ty_int) {
				EM_error(a->pos, "The right part of binary operation must be an integer!");
			}
			if (left_ty->kind != Ty_int) {
				EM_error(a->pos, "The left part of binary operation must be an integer!");
			}
			//#TODO error handle
			te = Tr_arithExp(oper, left_expty.exp, right_expty.exp);
			return expTy(te, Ty_Int());

		case A_ltOp:case A_leOp:case A_gtOp:case A_geOp:
			// must be integers
			if (right_ty->kind != Ty_int) {
				EM_error(a->pos, "The right part of binary operation must be an integer!");
			}
			if (left_ty->kind != Ty_int) {
				EM_error(a->pos, "The left part of binary operation must be an integer!");
			}
			//#TODO error handle
			te = Tr_logicExp(oper, left_expty.exp, right_expty.exp, FALSE);
			return expTy(te, Ty_Int());
		case A_eqOp:case A_neqOp:
			// #bug 除了基本类型以外的类型的比较可能会出现bug
			if (right_ty->kind != left_ty->kind) {
				EM_error(a->pos, "The left part and the right part should be the same type!");
			}
			else {
				if (right_ty->kind == Ty_string && left_ty->kind == Ty_string) {
					te = Tr_logicExp(oper, left_expty.exp, right_expty.exp, TRUE);
				}
				else {
					te = Tr_logicExp(oper, left_expty.exp, right_expty.exp, FALSE);
				}
			}

			return expTy(te, Ty_Int());
		}
		break;
	}
	case A_recordExp: {
		Ty_ty ty;
		Ty_fieldList tyfList;
		int cnt;
		A_efieldList eFields;
		A_efield a_efield;
		Ty_field tyField;
		struct expty expty;
		A_exp exp;
		int arg_number = EXACT_ARGS;
		ty = S_look(tenv, a->u.record.typ);
		int i;
		// 检查类型是否一致
		if (ty != NULL && ty->kind == Ty_record) {
			// 计算总数
			for (tyfList = ty->u.record, cnt = 0; tyfList != NULL; tyfList = tyfList->tail) {
				if (tyfList->head != NULL) {
					cnt++;
				}
			}
			te = Tr_recordExp_new(cnt);
			// 递归处理

			for (tyfList = ty->u.record, eFields = a->u.record.fields, i = 1;
			tyfList != NULL && eFields != NULL;
				tyfList = tyfList->tail, eFields = eFields->tail, i++) {
				tyField = tyfList->head;
				a_efield = eFields->head;
				if (tyField != NULL && a_efield != NULL) {
					if (tyField->name != a_efield->name) {
						EM_error(a_efield->exp->pos,
							"The field %d initialized for record '%s' is inconsistent with the declared field's name '%s'",
							i, S_name(a->u.record.typ), S_name(tyField->name));
					}
					else {
						// 校验声明
						expty = transExp(level, venv, tenv, a_efield->exp);
						if (expty.ty != tyField->ty) {
							EM_error(a->pos,
								"Type of field %d initialized for record '%s' is incompatible with the declared type",
								i, S_name(a->u.record.typ));
						}
						// Append the field initialization to the IR
						Tr_recordExp_app(te, expty.exp, i == cnt ? TRUE : FALSE);//Initialize next field
					}
				}
				else {
					//In case one list is run out, but the other not, an error occurs.
					if (a_efield == NULL && tyField != NULL) {
						arg_number = LESS_ARGS;
						break;
					}
					else if (a_efield != NULL && tyField == NULL) {
						arg_number = MORE_ARGS;
						break;
					}
				}
			}

			if (arg_number == MORE_ARGS) {
				EM_error(a->pos,
					"More than necessary fields are initialized for record '%s'", S_name(a->u.record.typ));
			}
			else if (arg_number == LESS_ARGS || (tyfList != NULL && tyfList->head != NULL)) {
				EM_error(a->pos,
					"Less than necessary fields are initialized for record '%s'", S_name(a->u.record.typ));
			}
			// 直接返回结果
			return expTy(te, ty);
		}
		else {
			EM_error(a->pos, "Undefined type '%s'", S_name(a->u.record.typ));
		}
		break;
	}
	case A_assignExp: {
		S_symbol var_sym = a->u.assign.var->u.simple;
		struct expty var_exp, assign_exp;
		// 检查变量是否在环境中声明过
		// 先计算左边，再计算右边
		var_exp = transVar(level, venv, tenv, a->u.assign.var);
		assign_exp = transExp(level, venv, tenv, a->u.assign.exp);

		// #TODO 更完善的类型检查
		if (actual_ty(var_exp.ty) != actual_ty(assign_exp.ty)) {
			EM_error(a->pos, "The left and right type is not the same!");
			// 当前只返回一个空的语句，不赋值任何内容
			return expTy(Tr_assignExp(var_exp.exp, NULL), Ty_Void());
		}
		return expTy(Tr_assignExp(var_exp.exp, assign_exp.exp), Ty_Void());
		break;
	}
	case A_seqExp: {
		// 计算总共有多少条语句
		int i = 0;
		A_expList expList;
		for (expList = a->u.seq; expList != NULL; expList = expList->tail) {
			if (expList->head != NULL) {
				i++;
			}
		}
		// decs
		struct expty resexpty;
		Tr_exp* te_array;
		A_exp exp;

		if (i == 0) { // 空语句
			resexpty = expTy(Tr_voidExp(), Ty_Void());
		}
		else {
			te_array = (Tr_exp*)checked_malloc(i*sizeof(struct Tr_exp_));
			for (expList = a->u.seq, i = 0; expList; expList = expList->tail) {
				// 进行逐语句的翻译
				exp = expList->head;
				if (exp != NULL) {
					resexpty = transExp(level, venv, tenv, exp);
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
		struct expty test_exp = transExp(level, venv, tenv, a->u.iff.test);
		if (actual_ty(test_exp.ty)->kind != Ty_int) {
			EM_error(a->u.iff.test->pos, "The TEST expression is not a number");
		}
		// trans the true-branch
		struct expty true_exp = transExp(level, venv, tenv, a->u.iff.then);
		// trans the false-branch
		struct expty false_exp;
		bool if_then_else;
		if (a->u.iff.elsee != NULL) {
			if_then_else = TRUE;
			false_exp = transExp(level, venv, tenv, a->u.iff.elsee);
		}
		else {
			if_then_else = FALSE;
		}
		// 测试类型是否一致
		struct expty resexp_ty;
		if (!if_then_else) {
			if (true_exp.ty->kind != Ty_void) {
				EM_error(a->pos, "The if statment should return void!");
			}
			resexp_ty = expTy(Tr_ifExp(test_exp.exp, true_exp.exp, NULL), Ty_Void());
		}
		else {
			if (true_exp.ty->kind != false_exp.ty->kind) {
				EM_error(a->pos, "The if...else... statment should return void!");
			}
			resexp_ty = expTy(Tr_ifExp(test_exp.exp, true_exp.exp, false_exp.exp), Ty_Void());
		}
		return resexp_ty;
	}
	case A_whileExp: {
		// #TODO 如何实现一个while语句 - 这里可以使用自己的方法
		// 可以利用一个栈来实现，方便嵌套以及break的跳出 - 每次进入一个循环就在栈里push一个符号
		VL_push(NULL);
		Tr_genLoopDoneLabel(); // break的时候可以跳转至此处
		struct expty test_exp, body_exp;
		test_exp = transExp(level, venv, tenv, a->u.whilee.test);
		if (actual_ty(test_exp.ty)->kind != Ty_int) {
			EM_error(a->pos, "The test exp in WHILE statment must return an INT value.");
		}
		body_exp = transExp(level, venv, tenv, a->u.whilee.body);
		if (actual_ty(body_exp.ty)->kind != Ty_void) {
			EM_error(a->pos, "The body of WHILE statment must return void");
		}

		// while 语句结束，进行pop
		VL_pop();

		return expTy(Tr_whileExp(test_exp.exp, body_exp.exp), Ty_Void());
	}
	case A_forExp: {
		//break;
		// 需要注意的是，for语句中可能会定义新的变量，需要新的环境
		S_beginScope(venv);

		// 检查循环变量是否在外界被使用，如果是，那么就报错，否则压入循环的堆栈
		S_symbol var_sym = a->u.forr.var;
		if (VL_check(var_sym)) {
			EM_error(a->pos, "The name '%s' has been used in the outer variables", S_name(var_sym));
		}
		else {
			VL_push(var_sym);
		}
		Tr_genLoopDoneLabel(); // break的时候可以跳转至此处

							   // translate the buttom and the upper
		struct expty for_bottom_exp, for_upper_exp;
		for_bottom_exp = transExp(level, venv, tenv, a->u.forr.lo);
		for_upper_exp = transExp(level, venv, tenv, a->u.forr.hi);

		if (actual_ty(for_bottom_exp.ty)->kind != Ty_int || actual_ty(for_upper_exp.ty)->kind != Ty_int) {
			EM_error(a->pos, "The FOR expression must be integer");
		}

		// 进入新的变量环境
		//#TODO 完成Tr_allocLocal函数
		Tr_access tr_acc = Tr_allocLocal(level, FALSE);
		S_enter(venv, var_sym, E_VarEntry(tr_acc, Ty_Int()));
		struct expty var_exp, body_exp;

		var_exp = transVar(level, venv, tenv, A_SimpleVar(a->pos, var_sym));
		body_exp = transExp(level, venv, tenv, a->u.forr.body);
		if (actual_ty(body_exp.ty)->kind != Ty_void) {
			EM_error(a->u.forr.body->pos, "Value return form FOR stm should be void");
		}

		// 离开环境
		VL_pop();
		S_endScope(venv);
		return expTy(Tr_forExp(var_exp.exp, for_bottom_exp.exp, for_upper_exp.exp, body_exp.exp), Ty_Void());
	}

	case A_breakExp: {
		if (VL_isEmpty()) {
			EM_error(a->pos, "BREAK only avialiable in loop structure.");
		}
		return expTy(Tr_breakExp(), Ty_Void());
	}
	case A_letExp: {
		A_decList d;
		S_beginScope(venv);
		S_beginScope(tenv);
		int i = 0; // 用来记录一共有多少条定义
		for (d = a->u.let.decs; d; d = d->tail) {
			if (d->head != NULL) {
				i++;
			}
		}
		Tr_exp * te_array = (Tr_exp *)checked_malloc((i + 1)*sizeof(struct Tr_exp_));
		i = 0;

		// 逐个声明进行遍历
		for (d = a->u.let.decs; d; d = d->tail) {
			if (d->head != NULL) {
				te_array[i++] = transDec(level, venv, tenv, d->head);
			}
		}
		struct expty resexpty = transExp(level, venv, tenv, a->u.let.body);
		te_array[i] = resexpty.exp;
		resexpty.exp = Tr_seqExp(te_array, i + 1);
		free(te_array);

		S_endScope(tenv);
		S_endScope(venv);
		return resexpty;
	}
	case A_arrayExp: {
		// 首先进行类型检查
		Ty_ty ty = S_look(tenv, a->u.array.typ);
		if (ty != NULL) {
			ty = actual_ty(ty);
			if (ty->kind != Ty_array) {
				EM_error(a->pos, "The type need to be an array");
			}
		}
		else {
			EM_error(a->pos, "Array type doesn't exist");
		}
		struct expty resexpty, ele_expty;

		// 检查数组的长度是否是整数
		resexpty = transExp(level, venv, tenv, a->u.array.size);
		if (resexpty.ty->kind != Ty_int) {
			EM_error(a->pos, "The length of the array must be an integer");
		}
		ele_expty = transExp(level, venv, tenv, a->u.array.init);
		return expTy(Tr_arrayExp(resexpty.exp, ele_expty.exp), ty);
	}
	}

	assert(0);
}


Ty_ty actual_ty(Ty_ty ty) {
	if (ty->kind == Ty_name){
		return actual_ty(ty->u.name.ty);
	}
	return ty; //Ty_record, Ty_nil, Ty_int, Ty_string, Ty_array, Ty_void, ~Ty_name~
}

E_enventry E_VarEntry(Tr_access access, Ty_ty ty) {
	E_enventry entry = checked_malloc(sizeof(*entry));
	entry->kind = E_varEntry;
	entry->u.var.ty = ty;
	entry->u.var.access = access;
	return entry;
}

E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty result) {
	E_enventry entry = checked_malloc(sizeof(*entry));
	entry->kind = E_funEntry;
	entry->u.fun.formals = formals;
	entry->u.fun.result = result;
	entry->u.fun.level = level;
	entry->u.fun.label = label;
	return entry;
}

// base type env
S_table E_base_tenv(void) {
	S_table table = S_empty();
	S_enter(table, S_Symbol("int"), Ty_Int());
	S_enter(table, S_Symbol("string"), Ty_String());
	return table;
}

// base variable env
S_table E_base_venv(void) {
	S_table table = S_empty();
	return table;
}

static bool isReferToOutside(bool* marked, int marked_len, S_symbol* syms, S_symbol refered) {
	int i;
	for (i = 0; i < marked_len; i++) {
		if (syms[i] == refered) {
			//Found the referred type in the dec sequence
			return marked[i];
		}
	}
	return TRUE;
}

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params) {
	Ty_tyList tyList = Ty_TyList(NULL, NULL);
	Ty_tyList tyList_head = tyList;
	bool legal = TRUE;

	S_symbol _sym;
	Ty_ty ty;

	//Always complete the checking for the formals regardless of errors
	SL_empty();
	for (; params != NULL; params = params->tail) {
		A_field head = params->head;
		if (head == NULL) {
			continue;
		}
		if (VL_check(head->name)) {
			EM_error( head->pos, "The name of loop variant '%s' cannot be reused by function's argument", S_name(head->name));
		}
		// 检查是否被定义
		_sym = head->typ;
		ty = S_look(tenv, _sym);
		if (ty == NULL) {
			EM_error(head->pos, "Undefined type: %s", S_name(_sym));
			legal = legal && FALSE;
		}
		// 名字出现过?
		if (!SL_check(head->name)) {
			SL_push(head->name);
		}
		else {
			EM_error(head->pos, "The name of formal argument '%s' has been used for this function", S_name(head->name));
		}
		if (legal) {
			tyList->tail = Ty_TyList(NULL, NULL);
			tyList->head = ty;
			tyList = tyList->tail;
		}
	}

	tyList->tail = NULL;

	return tyList_head->head != NULL ? tyList_head : NULL;
}
static Ty_ty transTyHeader(A_ty a) {
	S_symbol sym;
	Ty_ty ty;
	switch (a->kind) {//A_nameTy, A_recordTy, A_arrayTy
	case A_nameTy:
		return Ty_Name(a->u.name, NULL);
	case A_recordTy:
		return Ty_Record(NULL);
	case A_arrayTy:
		return Ty_Array(NULL);
	}
}
void createEscapeList(U_boolList *formal_escs, A_fieldList fList) {
	if (fList == NULL) {
		*formal_escs = NULL;
		return;
	}

	U_boolList escs = NULL;
	U_boolList escs_head = NULL;
	for (; fList != NULL; fList = fList->tail) {
		if (escs_head == NULL) {
			escs = U_BoolList(TRUE, NULL);
			escs_head = escs;
		}
		else {
			escs->tail = U_BoolList(TRUE, NULL);
			escs = escs->tail;
		}
	}

	if (escs != NULL) {
		escs->tail = NULL;
	}

	*formal_escs = escs_head;
}