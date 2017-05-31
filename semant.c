#include "util.h"
#include "log.h"
#include "symbol.h"
#include "absyn.h"
#include "semant.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include "translate.h"

// local function dec
struct expty transVar(S_table venv, S_table tenv, A_var v);
struct expty transExp(S_table venv, S_table tenv, A_exp a);
Tr_exp transDec(S_table venv, S_table tenv, A_dec d);
Ty_ty transTy(S_table tenv, A_ty a);
Ty_ty actual_ty(Ty_ty ty); // return the actual type of ty

string TAG = "SEMANT module:";

// function for stack operations
/* Loop variants START */
static stack_node variant_list = NULL;//variant_node 

static bool pointerCompare(void *a, void *b) {
	return a == b ? TRUE : FALSE;
}
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
/* Loop variants END */

/* Symbol list (for generic use) START */
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
/* Symbol list (for generic use) END */

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
	string TAG = "TRANSEXP";
	if (a == NULL){
		// Log(TAG, "The exp for trasnExp is NULL."); // not an error
		return *dummy_expty_p;
	}

	Tr_exp te; // 总的表达式
	switch (a -> kind){
		case A_intExp:{
			te = Tr_intExp(a->u.intt);
			return expTy(te, Ty_Int());
		}
		case A_varExp: {
			return transVar(level, venv, tenv, a->u.var);
		}
		case A_nilExp: {
			te = Tr_nilExp();
			return expTy(te,Ty_Nil());
		}
		case A_stringExp: {
			te = Tr_stringExp(a->u.stringg);
			return expTy(te, Ty_String());
		}
		case A_callExp: {
			// #TODO unfinished function
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
				if (right_ty->kind != Ty_int){
					EM_error(a->pos, "The right part of binary operation must be an integer!");
				}
				if (left_ty->kind != Ty_int){
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
				if (right_ty->kind != left_ty->kind){
					EM_error(a->pos, "The left part and the right part should be the same type!");
					// #TODO error handle
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
			// #TODO an unimplement case
			break;
		}
		case A_assignExp: {
			S_symbol var_sym = a->u.assign.var->u.simple;
			// #TODO 检查此变量是否可以被赋值
			struct expty var_exp,assign_exp;
			// 检查变量是否在环境中声明过
			// 先计算左边，再计算右边
			var_exp = transVar(level, venv, tenv, a->u.assign.var);
			assign_exp = transExp(level, venv, tenv, a->u.assign.exp);

			// #TODO 更完善的类型检查
			if (actual_ty(var_exp.ty) != actual_ty(assign_exp.ty)){
				EM_error(a->pos, "The left and right type is not the same!");
				//#TODO 错误处理
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
			for (expList = a->u.seq; expList != NULL;expList = expList->tail) {
				if (expList->head != NULL){
					i++;
				}
			}
			// decs
			struct expty resexpty;
			Tr_exp* te_array;
			A_exp exp;

			if (i == 0){ // 空语句
				resexpty = expTy(Tr_voidExp(), Ty_Void());
			}
			else {
				te_array = (Tr_exp*)checked_malloc(i*sizeof(Tr_exp));
				for (expList = a->u.seq, i = 0; expList;expList = expList->tail) {
					// 进行逐语句的翻译
					exp = expList->head;
					if (exp != NULL){
						resexpty = transExp(level, venv, tenv, exp);
						te_array[i] = resexpty.exp;
						i++;
					}
				}
				resexpty = expTy(Tr_seqExp(te_array, i), resexpty.ty);
				free(te_array);
			}
			break;
		}
		case A_ifExp: {
			// 首先翻译测试条件判断语句
			struct expty test_exp = transExp(level, venv, tenv, a->u.iff.test);
			if (actual_ty(test_exp.ty) != Ty_int) {
				EM_error(a->u.iff.test->pos, "The TEST expression is not a number");
			}
			// trans the true-branch
			struct expty true_exp = transExp(level, venv, tenv, a->u.iff.then);
			// trans the false-branch
			struct expty false_exp;
			bool if_then_else;
			if (a->u.iff.elsee != NULL){
				if_then_else = TRUE;
				false_exp = transExp(level, venv, tenv, a->u.iff.elsee);
			}
			else {
				if_then_else = FALSE;
			}
			// 测试类型是否一致
			struct expty resexp_ty;
			if (!if_then_else){
				if (true_exp.ty->kind != Ty_void){
					EM_error(a->pos, "The if statment should return void!");
				}
				resexp_ty = expTy(Tr_ifExp(test_exp.exp, true_exp.exp, NULL), Ty_Void());
			}
			else {
				if (true_exp.ty->kind != false_exp.ty->kind){
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
			struct expty test_exp,body_exp;
			test_exp = transExp(level, venv, tenv, a->u.whilee.test);
			if (actual_ty(test_exp.ty) != Ty_int){
				EM_error(a->pos, "The test exp in WHILE statment must return an INT value.");
			}
			body_exp = transExp(level, venv, tenv, a->u.whilee.body);
			if (actual_ty(body_exp.ty) != Ty_void){
				EM_error(a->pos, "The body of WHILE statment must return void");
			}

			// while 语句结束，进行pop
			VL_pop();

			return expTy(Tr_whileExp(test_exp.exp, body_exp.exp), Ty_Void());
		}
		// 不应该出现For语句
		case A_forExp: {
			break;
			// 需要注意的是，for语句中可能会定义新的变量，需要新的环境
			S_beginScope(venv);

			// 检查循环变量是否在外界被使用，如果是，那么就报错，否则压入循环的堆栈
			S_symbol var_sym = a->u.forr.var;
			if (VL_check(var_sym)){
				EM_error(a->pos, "The name '%s' has been used in the outer variables", S_name(var_sym));
			}
			else{
				VL_push(var_sym); // #TODO finish VL_XXXX 操作
			}
			Tr_genLoopDoneLabel(); // break的时候可以跳转至此处

			// translate the buttom and the upper
			struct expty for_bottom_exp, for_upper_exp;
			for_bottom_exp = transExp(level, venv, tenv, a->u.forr.lo);
			for_upper_exp = transExp(level, venv, tenv, a->u.forr.hi);

			if (actual_ty(for_bottom_exp.ty) != Ty_int || actual_ty(for_upper_exp.ty) != Ty_int ) {
				EM_error(a->pos, "The FOR expression must be integer");
			}

			// 进入新的变量环境
			//#TODO 完成Tr_allocLocal函数
			Tr_access tr_acc = Tr_allocLocal(level, FALSE ); // #bug 总是不逃逸的？？
			S_enter(venv, var_sym, E_VarEntry(tr_acc, Ty_Int()));
			struct expty var_exp, body_exp;
			
			var_exp = transVar(level, venv, tenv, A_SimpleVar(a->pos, var_sym));
			body_exp = transExp(level, venv, tenv, a->u.forr.body);
			if (actual_ty(body_exp.ty)!= Ty_void){
				EM_error(a->u.forr.body->pos, "Value return form FOR stm should be void");
			}
			
			// 离开环境
			VL_pop();
			S_endScope(venv);
			return expTy(Tr_forExp(var_exp.exp, for_bottom_exp.exp, for_upper_exp.exp, body_exp.exp), Ty_Void());
		}
		case A_breakExp:{
			if (VL_isEmpty()){
				EM_error(a->pos, "BREAK only avialiable in loop structure.");
			}
			return expTy(Tr_breakExp(), Ty_Void());
		}
		case A_letExp:{
			A_decList d;
			S_beginScope(venv);
			S_beginScope(tenv);
			int i = 0; // 用来记录一共有多少条定义
			for(d = a -> u.let.decs; d; d=d->tail){
				if (d->head != NULL){
					i++;
				}
			}
			Tr_exp * te_array = (Tr_exp *)checked_malloc((i + 1)*sizeof(Tr_exp));
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
			if (ty != NULL){
				ty = actual_ty(ty);
				if (ty->kind != Ty_array){
					EM_error(a->pos, "The type need to be an array");
				}
			}
			else {
				EM_error(a->pos, "Array type doesn't exist");
			}
			struct expty resexpty,ele_expty;

			// 检查数组的长度是否是整数
			resexpty = transExp(level, venv, tenv, a->u.array.size);
			if (resexpty.ty->kind != Ty_int){
				EM_error(a->pos, "The length of the array must be an integer");
			}
			ele_expty = transExp(level, venv, tenv, a->u.array.init);
			return expTy(Tr_arrayExp(resexpty.exp, ele_expty.exp), ty);
		}
	}

	// can't goes here unless some function is unimplemented
	assert(0);
}

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
					debug("transVar: A_fieldVar = %s", S_name(field->name));
					te = Tr_fieldVar(exp.exp, offset);
					return expTy(te, actual_ty(field->ty));
				}
			}
			EM_error(v->pos, "Undefined field '%s' in record variable", S_name(v->u.field.sym));
			break;
		}
		case A_subscriptVar: {
			//1) Translate the part ahead of '[', which should be an array
			exp = transVar(level, venv, tenv, v->u.subscript.var);
			if (exp.ty->kind != Ty_array) {
				EM_error(v->pos, "Indexed access to a non-array variable");
				break;
			}

			//2) Translate the part between [ and ], which should be an int
			exp2 = transExp(level, venv, tenv, v->u.subscript.exp);
			if (exp2.ty->kind != Ty_int) {
				EM_error( v->pos, "Indexed access to array variable must be of integer type");
				break;
			}
			te = Tr_arrayVar(exp.exp, exp2.exp);

			//3) Get the type of array's element
			return expTy(te, actual_ty(exp.ty->u.array));
		}
	}

}

Tr_exp transDec(Tr_level level, S_table venv, S_table tenv, A_dec d) {
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

Ty_ty actual_ty(Ty_ty ty) {
	if (ty->kind == Ty_name){
		return actual_ty(ty->u.name.ty);
	}
	return ty; //Ty_record, Ty_nil, Ty_int, Ty_string, Ty_array, Ty_void, ~Ty_name~
}