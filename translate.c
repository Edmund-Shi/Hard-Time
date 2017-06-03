/* translate.c
 * 中间代码生成的关键文件，主要作用是将抽象语法树转换成 IR Tree。
 * 主要的功能是处理很多种复杂的情况。
 * 对 translate.h 中声明的函数进行实现
 */
#include <stdio.h>
#include "translate.h"
#include "tree.h"
#include "frame.h"
#include "semant.h"

static patchList PatchList(Temp_label *_head, patchList _tail) {
	patchList list = (patchList)checked_malloc(sizeof(*list));
	list->head = _head;
	list->tail = _tail;
	return list;
}

//三个构造函数的实现
static Tr_exp Tr_Ex(T_exp _ex) {
	Tr_exp exp = (Tr_exp)checked_malloc(sizeof(*exp));
	exp->kind = Tr_ex;
	exp->u.ex = _ex;
	return exp;
}
static Tr_exp Tr_Nx(T_stm _nx) {
	Tr_exp exp = (Tr_exp)checked_malloc(sizeof(*exp));
	exp->kind = Tr_nx;
	exp->u.nx = _nx;
	return exp;
}
static Tr_exp Tr_Cx(patchList _t, patchList _f, T_stm _stm) {
	Tr_exp exp = (Tr_exp)checked_malloc(sizeof(*exp));
	exp->kind = Tr_cx;
	exp->u.cx.falses = _f;
	exp->u.cx.trues = _t;
	exp->u.cx.stm = _stm;
	return exp;
}
//三个转换函数的实现,借助了一个辅助函数 doPatch
void doPatch(patchList _list, Temp_label _label) {
	for (; _list; _list = _list->tail)
		*(_list->head) = _label;
}
static T_exp unEx(Tr_exp _e) {
	switch (_e->kind) {
	case Tr_ex:
		return _e->u.ex;
	case Tr_cx: {
		Temp_temp r = Temp_newtemp();
		Temp_label t = Temp_newlabel(), f = Temp_newlabel();
		doPatch(_e->u.cx.trues, t);
		doPatch(_e->u.cx.falses, f);
		return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
			T_Eseq(_e->u.cx.stm,
				T_Eseq(T_Label(f),
					T_Eseq(T_Move(T_Temp(r), T_Const(0)),
						T_Eseq(T_Label(t),
							T_Temp(r))))));
	}
	case Tr_nx: {
		return T_Eseq(_e->u.nx, T_Const(0));
	}
	}
}
static T_stm unNx(Tr_exp _e) {
	switch (_e->kind) {
	case Tr_nx: { return _e->u.nx; }
	case Tr_ex: { return T_Exp(_e->u.ex); }
	case Tr_cx: {
		Temp_label t, f;
		t = Temp_newlabel(); f = Temp_newlabel();
		doPatch(_e->u.cx.trues, t);	doPatch(_e->u.cx.falses, f);
		return T_Seq(_e->u.cx.stm, T_Seq(T_Label(t), T_Label(f)));
	}
	}
}
static struct Cx unCx(Tr_exp _e) {
	switch (_e->kind) {
	case Tr_ex: {
		struct Cx cx;
		cx.stm = T_Cjump(T_ne, _e->u.ex, T_Const(0), NULL, NULL);
		cx.trues = PatchList(&cx.stm->u.CJUMP.true, NULL);
		cx.falses = PatchList(&cx.stm->u.CJUMP.false, NULL);
		return cx;
	}
	case Tr_cx: { return _e->u.cx; }
	case Tr_nx: {
		struct Cx cx;
		cx.trues = cx.falses = NULL;
		cx.stm = _e->u.nx;
		return cx;
	}
	}
}
//单独的一个变量存储栈帧信息
static Tr_level outLevel = NULL;
//获得最外层的层级结构
Tr_level Tr_outLevel() {
	//如果最外层的栈帧还未被创建
	if (outLevel == NULL) {
		outLevel = (Tr_level)checked_malloc(sizeof(*outLevel));
		outLevel->parent = NULL;
		outLevel->depth = 0;
		outLevel->frame = F_newFrame(Temp_namedlabel("_main"), NULL);
	}
	return outLevel;
}
//根据信息创造一个新的层级
Tr_level Tr_newLevel(Tr_level _parent, Temp_label _name, U_boolList _formals) {
	Tr_level level = (Tr_level)checked_malloc(sizeof(*level));
	level->parent = _parent;
	level->frame = F_newFrame(_name, U_BoolList(TRUE, _formals));
	level->depth = _parent->depth + 1;
	return level;
}
//根据层级得到该层可以接触的所有变量链表
Tr_accessList Tr_formals(Tr_level _level) {
	Tr_accessList aList = NULL, aListHead = NULL;
	F_accessList fList = F_formals(_level->frame);
	while (fList != NULL) {
		if (aListHead == NULL) {
			aList = (Tr_accessList)checked_malloc(sizeof(*aList));
			aListHead = aList;
		}
		else {
			aList->tail = (Tr_accessList)checked_malloc(sizeof(*(aList->tail)));
			aList = aList->tail;
		}
		aList->head = (Tr_access)checked_malloc(sizeof(struct Tr_access_));
		aList->head->level = _level;
		aList->head->access = fList->head;
		aList->tail = NULL;
		fList = fList->tail;
	}
	return aListHead;
}
//得到某一个变量的具体接触信息
Tr_access Tr_allocLocal(Tr_level _level, bool _escape) {
	F_access fa = F_allocLocal(_level->frame, _escape);
	Tr_access tracc = (Tr_access)checked_malloc(sizeof(*tracc));
	tracc->level = _level;
	tracc->access = fa;
	return tracc;
}
//得到一个层级的父层
Tr_level Tr_getParent(Tr_level _level) {
	return _level->parent;
}
//简单变量的识别
Tr_exp Tr_simpleVar(Tr_access _ac, Tr_level _le) {
	F_access access = _ac->access;
	T_exp exp;
	if (_le == _ac->level) {
		exp = F_Exp(access, T_Temp(F_FP()));
	}
	else {
		exp = F_exp(F_staticLink(), T_Temp(F_FP()));
		_le = _le->parent;
		while (_le != _ac->level) {
			_le = _le->parent;
			exp = F_Exp(F_staticLink(), exp);
		}
		exp = F_Exp(access, exp);
	}
	return Tr_Ex(exp);
}
//对于数组变量的识别，输入一个是基地址，一个是偏移量
Tr_exp Tr_arrayVar(Tr_exp _base, Tr_exp _offset) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(_base), T_Binop(T_mul, unEx(_offset), T_Const(F_wordSize)))));
}
//对于 if 语句的处理，分三种情况进行处理。
Tr_exp Tr_ifExp(Tr_exp _cond, Tr_exp _t, Tr_exp _f) {
	Tr_exp ret;
	struct Cx cx_cond = unCx(_cond);
	Temp_label t = Temp_newlabel();
	Temp_label f = Temp_newlabel();
	Temp_label join = Temp_newlabel();
	doPatch(cx_cond.trues, t);
	doPatch(cx_cond.falses, f);
	//没有 else 的情况
	if (_f == NULL) {
		T_stm st;
		st = T_Seq(cx_cond.stm, T_Seq(T_Label(t), T_Seq(unNx(_t), T_Label(f))));
		ret = Tr_Nx(st);
	}
	//两个分支语句都是 Nx 的情况
	else if (_t->kind == Tr_nx && _f->kind == Tr_nx) {
		T_stm st;
		st = T_Seq(cx_cond.stm, T_Seq(T_Label(t), T_Seq(unNx(_t), T_Seq(T_Jump(T_Name(join), Temp_LabelList(join, NULL)), T_Seq(T_Label(f), T_Seq(unNx(_f), T_Label(join)))))));
		ret = Tr_Nx(st);
	}
	//更加复杂的情况
	else {
		T_exp ex;
		Temp_temp r = Temp_newtemp();
		ex = T_Eseq(cx_cond.stm, T_Eseq(T_Label(t), T_Eseq(T_Move(T_Temp(r), unEx(_t)), T_Eseq(T_Jump(T_Name(join), Temp_LabelList(join, NULL)), T_Eseq(T_Label(f), T_Eseq(T_Move(T_Temp(r), unEx(_f)), T_Eseq(T_Label(join), T_Temp(r))))))));
		ret = Tr_Ex(ex);
	}
	return ret;
}
//对于字符串的处理
Tr_exp Tr_stringExp(string _str) {
	T_expList list = (T_expList)checked_malloc(sizeof(struct T_expList_));
	T_expList head = list;
	char *p = _str;
	while (*p != '\0') {
		list->head = T_Const((int)*p);
		list->tail = (Tr_exp)checked_malloc(sizeof(struct T_expList_));
		list = list->tail;
		p++;
	}
	return Tr_Ex(F_externalCall("initString", head));
}
//处理记录的创建，调用 malloc 函数并绑定一个临时变量
Tr_exp Tr_recordExp_new(int _count) {
	T_exp call = F_externalCall("malloc", T_ExpList(T_Const(_count*F_wordSize), NULL));
	Temp_temp r = Temp_newtemp();
	T_exp ex = T_Eseq(T_Seq(T_Move(T_Temp(r), call), NULL), T_Temp(r));
	return Tr_Ex(ex);
}
//处理记录中的每一个元素
void Tr_recordExp_app(Tr_exp _exp, Tr_exp _init, bool _final) {
	T_exp ex = unEx(_exp);
	T_stm *right = &(ex->u.ESEQ.stm->u.SEQ.right);
	int i = 0;
	//找到当前的元素
	while (*right != NULL) {
		right = &((*right)->u.SEQ.right);
		i++;
	}
	//对元素进行处理
	T_stm move = T_Move(T_Mem(T_Binop(T_plus, ex->u.ESEQ.exp, T_Const(i*F_wordSize))), unEx(_init));
	//如果不是最后一个元素，那么应该给下一个元素留出位置
	if (!_final) *right = T_Seq(move, NULL);
	else *right = move;
}
//数组的创建和初始化，直接调用函数 initArray
Tr_exp Tr_arrayExp(Tr_exp _size, Tr_exp _val) {
	return Tr_Ex(F_externalCall("initArray", T_ExpList(unEx(_size), T_ExpList(unEx(_val), NULL))));
}
//循环的处理
//先是对于嵌套 Label 的处理，使用一个堆栈，堆栈的实现比较简单
static stack_node loopLabelList = NULL;
static void LL_push(Temp_label _label) {
	GS_push(&loopLabelList, _label);
}
static void LL_pop() {
	GS_pop(&loopLabelList);
}
static Temp_label LL_peek() {
	return GS_peek(&loopLabelList);
}
//然后是生成一个结束 Label，就是生成一个 Label 到堆栈中
void Tr_genLoopDoneLabel() {
	Temp_label l = Temp_newlabel();
	LL_push(l);
}
//while 循环的处理
Tr_exp Tr_whileExp(Tr_exp _cond, Tr_exp _body) {
	T_stm st;
	//f 对应的 Label 应该在函数调用时已经 Push 进去
	Temp_label t = Temp_newlabel(), f = LL_peek();
	Temp_label start = Temp_newlabel();
	struct Cx cx_cond = unCx(_cond);
	doPatch(cx_cond.trues, t);
	doPatch(cx_cond.falses, f);
	//生成对应的类似于条件语句的结构
	st = T_Seq(T_Label(start), T_Seq(cx_cond.stm, T_Seq(T_Label(t),	T_Seq(unNx(_body), T_Seq(T_Jump(T_Name(start),Temp_LabelList(start, NULL)),T_Label(f))))));
	//在 While 循环结束时，将这个标号 Pop 出来。
	LL_pop();
	return Tr_Nx(st);
}
//for 循环的处理（将 for 循环转换成 while 循环的形式）
Tr_exp Tr_forExp(Tr_exp _var, Tr_exp _l, Tr_exp _h, Tr_exp _body) {
	T_stm st;
	T_exp v = unEx(_var);
	Temp_label t = Temp_newlabel(), f = LL_peek();
	Temp_label start = Temp_newlabel();
	//创建一个类似于 while 循环。
	T_stm cond = T_Cjump(T_le, v, unEx(_h), t, f);
	st = T_Seq(T_Move(v, unEx(_l)), T_Seq(T_Label(start), T_Seq(cond, T_Seq(T_Label(t), T_Seq(unNx(_body), T_Seq(T_Move(v, T_Binop(T_plus, v, T_Const(1))), T_Seq(T_Jump(T_Name(start), Temp_LabelList(start, NULL)), T_Label(f))))))));
	//在 For 循环结束时，将这个标号 Pop 出来。
	LL_pop();
	return Tr_Nx(st);
}
//break 的处理，找到对应的 done 的标签，然后跳转
Tr_exp Tr_breakExp() {
	Temp_label f = LL_peek();
	T_stm st = T_Jump(T_Name(f), Temp_LabelList(f, NULL));
	return Tr_Nx(st);
}
//处理函数调用
Tr_exp Tr_callExp(Tr_level _callerLevel, Tr_level _calleeLevel, Temp_label _funLabel, Tr_exp* _argv, int _args) {
	int z = 0, cnt = 0;
	//计算静态链
	T_exp slk = F_Exp(F_staticLink(), T_Temp(F_FP()));;
	if (_callerLevel != _calleeLevel) {
		_callerLevel = _callerLevel->parent;
		while (_callerLevel != _calleeLevel) {
			slk = F_Exp(F_staticLink(), slk);
			_callerLevel = _callerLevel->parent;
		}
	}
	//计算参数列表，当参数不为0时，静态链同样应该作为一个参数
	T_expList listp_head = NULL;
	if (_args > 0) {
		T_expList listp = (T_expList)checked_malloc(sizeof(struct T_expList_));
		listp_head = listp;
		listp->head = slk;
		listp->tail = NULL;
		int i;
		for (i = 0; i < _args; i++, _argv++) {
			listp->tail = (T_expList)checked_malloc(sizeof(struct T_expList_));
			listp = listp->tail; listp->head = unEx(*_argv);
			listp->tail = NULL;
		}
	}
	//生成表达式
	T_exp exp = T_Call(T_Name(_funLabel), listp_head);
	return Tr_Ex(exp);
}
//声明为空（前者为存储器为空，第二个是值为空）
Tr_exp Tr_nilExp() { return Tr_Ex(T_Mem(T_Const(0))); }
Tr_exp Tr_voidExp(void) { return Tr_Ex(T_Const(0)); }
//整型常量的处理
Tr_exp Tr_intExp(int _val) { return Tr_Ex(T_Const(_val)); }
//二元计算操作
Tr_exp Tr_arithExp(A_oper _oper, Tr_exp _left, Tr_exp _right) {
	switch (_oper) {
	case A_plusOp: return Tr_Ex(T_Binop(T_plus, unEx(_left), unEx(_right)));
	case A_minusOp: return Tr_Ex(T_Binop(T_minus, unEx(_left), unEx(_right)));
	case A_timesOp:	return Tr_Ex(T_Binop(T_mul, unEx(_left), unEx(_right)));
	case A_divideOp: return Tr_Ex(T_Binop(T_div, unEx(_left), unEx(_right)));
	}
}
//二元逻辑操作(如果是字符串比较，则单独处理）
Tr_exp Tr_logicExp(A_oper _op, Tr_exp _l, Tr_exp _r, bool _isStr) {
	T_stm stm;
	//如果是字符串比较，则调用一个函数
	if (_isStr) {
		T_exp call = F_externalCall("stringEqual", T_ExpList(unEx(_l), T_ExpList(unEx(_r), NULL)));
		if (_op == A_eqOp)stm = T_Cjump(T_eq, call, T_Const(1), NULL, NULL);
		else stm = T_Cjump(T_eq, call, T_Const(0), NULL, NULL);
	}
	//如果不是，则直接利用 T_Cjump 函数构造
	else {
		if (_op == A_eqOp)stm = T_Cjump(T_eq, unEx(_l), unEx(_r), NULL, NULL);
		else if (_op == A_neqOp)stm = T_Cjump(T_ne, unEx(_l), unEx(_r), NULL, NULL);
		else if (_op == A_ltOp)stm = T_Cjump(T_lt, unEx(_l), unEx(_r), NULL, NULL);
		else if (_op == A_gtOp)stm = T_Cjump(T_gt, unEx(_l), unEx(_r), NULL, NULL);
		else if (_op == A_leOp)stm = T_Cjump(T_le, unEx(_l), unEx(_r), NULL, NULL);
		else stm = T_Cjump(T_ge, unEx(_l), unEx(_r), NULL, NULL);
	}
	patchList tl = PatchList(&stm->u.CJUMP.true, NULL);
	patchList fl = PatchList(&stm->u.CJUMP.false, NULL);
	return Tr_Cx(tl, fl, stm);
}
//赋值操作
Tr_exp Tr_assignExp(Tr_exp _l, Tr_exp _r) {
	if (_r != NULL) return Tr_Nx(T_Move(unEx(_l), unEx(_r)));
	else return _l;
}
//定义记录变量
Tr_exp Tr_fieldVar(Tr_exp _base, int _offset) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(_base), T_Const(_offset * F_wordSize))));
}
//多个语句拆分
Tr_exp Tr_seqExp(Tr_exp* _array, int _size) {
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	T_exp *p = &exp, head;
	int i, last = _size - 1;
	for (i = 0; i < _size; i++) {
		//如果不是最后一个，要给后面的留出位置
		if (i != last) *p = T_Eseq(unNx(_array[i]), NULL);
		else *p = unEx(_array[i]);
		if (i == 0) head = *p;
		if (i != last) p = &((*p)->u.ESEQ.exp);
	}
	return Tr_Ex(head);
}
//获取最终的结果
T_stm getResult(struct expty _res){
	return unNx(_res.exp);
}



