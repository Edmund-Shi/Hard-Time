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

S_table E_base_tenv();
S_table E_base_venv();

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

	S_table venv = E_base_venv();
	S_table tenv = E_base_tenv();

	Tr_level mainlevel = Tr_newLevel(Tr_outermost(), Temp_namedlabel("main"), NULL);
	struct expty result = transExp(mainlevel, venv, tenv, exp);
	
	// #TODO print IR tree
	return result;
}
#define EXACT_ARGS 0
#define LESS_ARGS 1
#define MORE_ARGS 2
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
			Ty_ty ty;
			Ty_fieldList tyfList;
			int cnt;
			A_efieldList eFields;
			A_efield a_efield;
			Ty_field tyField;
			struct expty expty;
			A_exp exp;
			int arg_number;
			ty = S_look(tenv, a->u.record.typ);
			int i;
			//1) Check if the record expression's leading ID is a declared record type
			if (ty != NULL && ty->kind == Ty_record) {
				//1) Calculate the number of fields
				for (tyfList = ty->u.record, cnt = 0; tyfList != NULL; tyfList = tyfList->tail) {
					if (tyfList->head != NULL) {
						cnt++;
					}
				}
				//debug("record fields in total: %d", cnt);
				te = Tr_recordExp_new(cnt);

				//2) Recursively check each field
				
				for (tyfList = ty->u.record, eFields = a->u.record.fields, i = 1;
				tyfList != NULL && eFields != NULL;
					tyfList = tyfList->tail, eFields = eFields->tail, i++) {
					tyField = tyfList->head;
					a_efield = eFields->head;
					//debug("record field");
					if (tyField != NULL && a_efield != NULL) {
						//debug("record field init: %d", i);
						//2.1) Check field name's consistency
						if (tyField->name != a_efield->name) {
							EM_error(a_efield->exp->pos,
								"The field %d initialized for record '%s' is inconsistent with the declared field's name '%s'",
								i, S_name(a->u.record.typ), S_name(tyField->name));
						}
						else {
							//2.2) Check the compatibility between the declared field type and that of the initializer
							expty = transExp(level, venv, tenv, a_efield->exp);
							if ( expty.ty != tyField->ty) {
								EM_error( exp->pos,
									"Type of field %d initialized for record '%s' is incompatible with the declared type",
									i, S_name(a->u.record.typ));
							}
							//2.3) Append the field initialization to the IR
							//debug("             type: %s", z_str_ty[expty.ty->kind]);
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
					EM_error( a->pos,
						"Less than necessary fields are initialized for record '%s'", S_name(a->u.record.typ));
				}

				//Terminate in case of any error
		

				//3) Return the result
				return expTy(te, ty);
			}
			else {
				EM_error( a->pos, "Undefined type '%s'", S_name(a->u.record.typ));
			}
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
			if (actual_ty(test_exp.ty)->kind != Ty_int) {
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
			if (actual_ty(test_exp.ty)->kind != Ty_int){
				EM_error(a->pos, "The test exp in WHILE statment must return an INT value.");
			}
			body_exp = transExp(level, venv, tenv, a->u.whilee.body);
			if (actual_ty(body_exp.ty)->kind != Ty_void){
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

			if (actual_ty(for_bottom_exp.ty)->kind != Ty_int || actual_ty(for_upper_exp.ty)->kind != Ty_int ) {
				EM_error(a->pos, "The FOR expression must be integer");
			}

			// 进入新的变量环境
			//#TODO 完成Tr_allocLocal函数
			Tr_access tr_acc = Tr_allocLocal(level, FALSE ); // #bug 总是不逃逸的？？
			S_enter(venv, var_sym, E_VarEntry(tr_acc, Ty_Int()));
			struct expty var_exp, body_exp;
			
			var_exp = transVar(level, venv, tenv, A_SimpleVar(a->pos, var_sym));
			body_exp = transExp(level, venv, tenv, a->u.forr.body);
			if (actual_ty(body_exp.ty)->kind != Ty_void){
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
	Ty_ty resultTy;
	Ty_tyList formalTys;
	E_enventry entry;
	A_fieldList fList;
	Ty_tyList tList;
	struct expty e;
	Ty_ty varDecTy;
	Ty_ty ty;

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
				EM_error(d->pos, 
					"The initializer of variable '%s' is incompatible with the declared type", S_name(var_sym));
			}
		}
		else {
			if (ty->kind == Ty_nil) {
				EM_error(d->pos, "Cannot inference the type of variable '%s' as NIL is used as initializer", S_name(var_sym));
			}
			varDecTy = e.ty;
		}
	
		tr_acc = Tr_allocLocal(level, TRUE);//always escaping
		S_enter(venv, var_sym, E_VarEntry(tr_acc, varDecTy));
		tr_exp = Tr_assignExp(transVar(level, venv, tenv, A_SimpleVar(d->pos, d->u.var.var)).exp, e.exp);
		break;
	}
	case A_typeDec: {
		//1) Put header of type declarations into the environment for 
		//recursive reference during next step.
		//1.1) Also check if there are types of same name. In a mutually 
		//recursive declaration block no two types may have the same name.
		//(namely no re-declaration)
		SL_empty();
		for (types = d->u.type; types != NULL; types = types->tail) {
			type = types->head;
			if (type != NULL) {
				//1) Check if it's been used in this block
				if (SL_check(type->name)) {
					EM_error(d->pos,
						"The type name '%s' has been used adjacently",
						S_name(type->name));
				}
				else {
					SL_push(type->name);
				}

				S_enter(tenv, type->name, transTyHeader(type->ty));

				//Prepare for 3) - see below
				if (type->ty->kind == A_nameTy) {
					names++;
				}
			}
		}

		//Prepare for 3) - see below
		nameTys = checked_malloc(sizeof(A_ty)*names);
		tySymbols = checked_malloc(sizeof(S_symbol)*names);
		marked = checked_malloc(sizeof(bool)*names);
		for (ind = 0; ind < names; ind++) {
			marked[ind] = FALSE;
		}
		marked_len = names;
		names = 0;

		//2) Translate type declarations: instantiate the body of Ty_ty
		//created by transTyHeader().
		for (types = d->u.type; types != NULL; types = types->tail) {
			type = types->head;
			if (type != NULL) {
				transTy(tenv, type->name, type->ty);

				//Prepare for 3) - see below
				if (type->ty->kind == A_nameTy) {
					tySymbols[names] = type->name;
					nameTys[names++] = type->ty;
				}
			}
		}

		//3) Inspect cyclic references
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

		//4) Print out the types [WARNING: Infinite loop]
		/*
		for(types = d->u.type; types!=NULL; types=types->tail){
		if(types->head != NULL){
		z_Ty_print(S_look(tenv, types->head->name), 0);
		}
		}
		*/
		tr_exp = Tr_voidExp();//Return NO_OP
		break;
	}
	
	case A_functionDec: {
		SL_empty();
		for (functions = d->u.function; functions != NULL; functions = functions->tail) {
			head = functions->head;
			if (head != NULL) {

				//1.1) Check if it's been used in this block
				if (SL_check(head->name)) {
					EM_error(d->pos,
						"The function name '%s' has been used adjacently",
						S_name(head->name));
				}
				else {
					SL_push(head->name);
				}

				//1.2) Determine the type of returned value
				if (head->result != NULL) {
					resultTy = S_look(tenv, head->result);
				}
				else {
					resultTy = Ty_Void();
				}

				//1.3) Solve the formal parameter list
				formalTys = makeFormalTyList(tenv, head->params);

				//1.4) Enter a new function into env
				//1.4.1) Create level
				createEscapeList(&formal_escs, head->params);
				funLevel = Tr_newLevel(level, Temp_namedlabel(S_name(head->name)), formal_escs);
				free(formal_escs);
				//1.4.2) Update env
				S_enter(venv, head->name, E_FunEntry(funLevel, Temp_newlabel(), formalTys, resultTy));
			}
		}

		//2) Translate function declarations.
		for (functions = d->u.function; functions != NULL; functions = functions->tail) {
			head = functions->head;
			if (head != NULL) {
				debug("  A_funDec translate body: %s", S_name(head->name));
				//2.1) retrieve the formals of this E_FunEntry
				entry = S_look(venv, head->name);
				formalTys = entry->u.fun.formals;

				//2.2) start new scope
				S_beginScope(venv);

				//2.3) add all parameters to venv as declared variables
				//Use tail here since the first argument is the static link.
				tr_formals = Tr_formals(entry->u.fun.level)->tail;
				for (fList = head->params, tList = formalTys; fList != NULL;
				fList = fList->tail, tList = tList->tail, tr_formals = tr_formals->tail) {
					S_enter(venv, fList->head->name, E_VarEntry(tr_formals->head, tList->head));
				}
				//2.4) translate the body, which is an exp
				e = transExp(entry->u.fun.level, venv, tenv, head->body);

				//2.5) check the type compatibility between body and the declared returned value
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
					//2.5.2) it's a procedure
					if (actual_ty(e.ty)->kind != Ty_void) {
						EM_error(head->pos,
							"The returned value of procedure '%s' will be ignored",
							S_name(head->name));
					}
				}

				//2.6) Translate to fragment
				Tr_procEntryExit(entry->u.fun.level, e.exp, Tr_formals(entry->u.fun.level), entry->u.fun.label);

				//2.7) exit the scope
				S_endScope(venv);
			}
		}

		tr_exp = Tr_voidExp();//Return NO_OP

		break;
	}
	
	}
	return tr_exp;
}
Ty_ty transTy(S_table tenv,S_symbol sym, A_ty a){
	A_fieldList flist;
	Ty_fieldList tyList = Ty_FieldList(NULL, NULL);
	Ty_fieldList tyList_head = tyList;

	//Get entry of this type from env. Should have been entered by transTyHeader() 
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
		//Terminate in case of error

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
	// #TODO 添加各种函数。。。
	return table;
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
		//1) Check if the variable has been used as loop variant
		if (VL_check(head->name)) {
			EM_error( head->pos, "The name of loop variant '%s' cannot be reused by function's argument", S_name(head->name));
		}
		//2) Check if the type for this argument has been declared
		_sym = head->typ;
		ty = S_look(tenv, _sym);
		if (ty == NULL) {
			EM_error(head->pos, "Undefined type: %s", S_name(_sym));
			legal = legal && FALSE;
		}
		//3) Check if the formal's name has been used in the list
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

	//Terminate if any error occured before in the loop

	return tyList_head->head != NULL ? tyList_head : NULL;
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