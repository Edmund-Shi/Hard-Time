#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
//表达式构造函数的定义，就是直接构造并赋值
T_exp T_Const(int _consti) {
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	exp->kind = T_CONST;
	exp->u.CONST = _consti;
	return exp;
}
T_exp T_Name(Temp_label _name) {
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	exp->kind = T_NAME;
	exp->u.NAME = _name;
	return exp;
}
T_exp T_Temp(Temp_temp _temp) {
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	exp->kind = T_TEMP;
	exp->u.TEMP = _temp;
	return exp;
}
T_exp T_Binop(T_binOp _op, T_exp _left, T_exp _right){
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	exp->kind = T_BINOP;
	exp->u.BINOP.op = _op;
	exp->u.BINOP.left = _left;
	exp->u.BINOP.right = _right;
	return exp;
}
T_exp T_Mem(T_exp _exp) {
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	exp->kind = T_MEM;
	exp->u.MEM = _exp;
	return exp;
}
T_exp T_Call(T_exp _fun, T_expList _args) {
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	exp->kind = T_CALL;
	exp->u.CALL.fun = _fun;
	exp->u.CALL.args = _args;
	return exp;
}
T_exp T_Eseq(T_stm _stm, T_exp _exp) {
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	exp->kind = T_ESEQ;
	exp->u.ESEQ.stm = _stm;
	exp->u.ESEQ.exp = _exp;
	return exp;
}

//语句构造函数的定义
T_stm T_Move(T_exp _dst, T_exp _src) {
	T_stm stm = (T_stm)checked_malloc(sizeof(*stm));
	stm->kind = T_MOVE;
	stm->u.MOVE.dst = _dst;
	stm->u.MOVE.src = _src;
	return stm;
}
T_stm T_Exp(T_exp _exp) {
	T_stm stm = (T_stm)checked_malloc(sizeof(*stm));
	stm->kind = T_EXP;
	stm->u.EXP = _exp;
	return stm;
}
T_stm T_Jump(T_exp _exp, Temp_labelList _labels) {
	T_stm stm = (T_stm)checked_malloc(sizeof(*stm));
	stm->kind = T_JUMP;
	stm->u.JUMP.exp = _exp;
	stm->u.JUMP.jumps = _labels;
	return stm;
}
T_stm T_Cjump(T_relOp _op, T_exp _left, T_exp _right,
	Temp_label _true, Temp_label _false) {
	T_stm stm = (T_stm)checked_malloc(sizeof(*stm));
	stm->kind = T_CJUMP;
	stm->u.CJUMP.op = _op; stm->u.CJUMP.left = _left; stm->u.CJUMP.right = _right;
	stm->u.CJUMP.true = _true;
	stm->u.CJUMP.false = _false;
	return stm;
}
T_stm T_Seq(T_stm _left, T_stm _right) {
	T_stm stm = (T_stm)checked_malloc(sizeof(*stm));
	stm->kind = T_SEQ;
	stm->u.SEQ.left = _left;
	stm->u.SEQ.right = _right;
	return stm;
}
T_stm T_Label(Temp_label _label) {
	T_stm stm = (T_stm)checked_malloc(sizeof(*stm));
	stm->kind = T_LABEL;
	stm->u.LABEL = _label;
	return stm;
}

//两种 List 构造函数的定义
T_expList T_ExpList(T_exp _head, T_expList _tail) {
	T_expList list = (T_expList)checked_malloc(sizeof *list);
	list->head = _head; list->tail = _tail;
	return list;
}
T_stmList T_StmList(T_stm _head, T_stmList _tail)
{
	T_stmList list = (T_stmList)checked_malloc(sizeof *list);
	list->head = _head; list->tail = _tail;
	return list;
}

T_relOp T_notRel(T_relOp _r)
{
	switch (_r)
	{
	case T_eq: return T_ne;
	case T_ne: return T_eq;
	case T_lt: return T_ge;
	case T_ge: return T_lt;
	case T_gt: return T_le;
	case T_le: return T_gt;
	case T_ult: return T_uge;
	case T_uge: return T_ult;
	case T_ule: return T_ugt;
	case T_ugt: return T_ule;
	}
	assert(0); return 0;
}
T_relOp T_commute(T_relOp _r)
{
	switch (_r) {
	case T_eq: return T_eq;
	case T_ne: return T_ne;
	case T_lt: return T_gt;
	case T_ge: return T_le;
	case T_gt: return T_lt;
	case T_le: return T_ge;
	case T_ult: return T_ugt;
	case T_uge: return T_ule;
	case T_ule: return T_uge;
	case T_ugt: return T_ult;
	}
	assert(0); return 0;
}


