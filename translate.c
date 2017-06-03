/* translate.c
 * �м�������ɵĹؼ��ļ�����Ҫ�����ǽ������﷨��ת���� IR Tree��
 * ��Ҫ�Ĺ����Ǵ���ܶ��ָ��ӵ������
 * �� translate.h �������ĺ�������ʵ��
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

//�������캯����ʵ��
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
//����ת��������ʵ��,������һ���������� doPatch
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
//������һ�������洢ջ֡��Ϣ
static Tr_level outLevel = NULL;
//��������Ĳ㼶�ṹ
Tr_level Tr_outLevel() {
	//���������ջ֡��δ������
	if (outLevel == NULL) {
		outLevel = (Tr_level)checked_malloc(sizeof(*outLevel));
		outLevel->parent = NULL;
		outLevel->depth = 0;
		outLevel->frame = F_newFrame(Temp_namedlabel("_main"), NULL);
	}
	return outLevel;
}
//������Ϣ����һ���µĲ㼶
Tr_level Tr_newLevel(Tr_level _parent, Temp_label _name, U_boolList _formals) {
	Tr_level level = (Tr_level)checked_malloc(sizeof(*level));
	level->parent = _parent;
	level->frame = F_newFrame(_name, U_BoolList(TRUE, _formals));
	level->depth = _parent->depth + 1;
	return level;
}
//���ݲ㼶�õ��ò���ԽӴ������б�������
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
//�õ�ĳһ�������ľ���Ӵ���Ϣ
Tr_access Tr_allocLocal(Tr_level _level, bool _escape) {
	F_access fa = F_allocLocal(_level->frame, _escape);
	Tr_access tracc = (Tr_access)checked_malloc(sizeof(*tracc));
	tracc->level = _level;
	tracc->access = fa;
	return tracc;
}
//�õ�һ���㼶�ĸ���
Tr_level Tr_getParent(Tr_level _level) {
	return _level->parent;
}
//�򵥱�����ʶ��
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
//�������������ʶ������һ���ǻ���ַ��һ����ƫ����
Tr_exp Tr_arrayVar(Tr_exp _base, Tr_exp _offset) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(_base), T_Binop(T_mul, unEx(_offset), T_Const(F_wordSize)))));
}
//���� if ���Ĵ���������������д���
Tr_exp Tr_ifExp(Tr_exp _cond, Tr_exp _t, Tr_exp _f) {
	Tr_exp ret;
	struct Cx cx_cond = unCx(_cond);
	Temp_label t = Temp_newlabel();
	Temp_label f = Temp_newlabel();
	Temp_label join = Temp_newlabel();
	doPatch(cx_cond.trues, t);
	doPatch(cx_cond.falses, f);
	//û�� else �����
	if (_f == NULL) {
		T_stm st;
		st = T_Seq(cx_cond.stm, T_Seq(T_Label(t), T_Seq(unNx(_t), T_Label(f))));
		ret = Tr_Nx(st);
	}
	//������֧��䶼�� Nx �����
	else if (_t->kind == Tr_nx && _f->kind == Tr_nx) {
		T_stm st;
		st = T_Seq(cx_cond.stm, T_Seq(T_Label(t), T_Seq(unNx(_t), T_Seq(T_Jump(T_Name(join), Temp_LabelList(join, NULL)), T_Seq(T_Label(f), T_Seq(unNx(_f), T_Label(join)))))));
		ret = Tr_Nx(st);
	}
	//���Ӹ��ӵ����
	else {
		T_exp ex;
		Temp_temp r = Temp_newtemp();
		ex = T_Eseq(cx_cond.stm, T_Eseq(T_Label(t), T_Eseq(T_Move(T_Temp(r), unEx(_t)), T_Eseq(T_Jump(T_Name(join), Temp_LabelList(join, NULL)), T_Eseq(T_Label(f), T_Eseq(T_Move(T_Temp(r), unEx(_f)), T_Eseq(T_Label(join), T_Temp(r))))))));
		ret = Tr_Ex(ex);
	}
	return ret;
}
//�����ַ����Ĵ���
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
//�����¼�Ĵ��������� malloc ��������һ����ʱ����
Tr_exp Tr_recordExp_new(int _count) {
	T_exp call = F_externalCall("malloc", T_ExpList(T_Const(_count*F_wordSize), NULL));
	Temp_temp r = Temp_newtemp();
	T_exp ex = T_Eseq(T_Seq(T_Move(T_Temp(r), call), NULL), T_Temp(r));
	return Tr_Ex(ex);
}
//�����¼�е�ÿһ��Ԫ��
void Tr_recordExp_app(Tr_exp _exp, Tr_exp _init, bool _final) {
	T_exp ex = unEx(_exp);
	T_stm *right = &(ex->u.ESEQ.stm->u.SEQ.right);
	int i = 0;
	//�ҵ���ǰ��Ԫ��
	while (*right != NULL) {
		right = &((*right)->u.SEQ.right);
		i++;
	}
	//��Ԫ�ؽ��д���
	T_stm move = T_Move(T_Mem(T_Binop(T_plus, ex->u.ESEQ.exp, T_Const(i*F_wordSize))), unEx(_init));
	//����������һ��Ԫ�أ���ôӦ�ø���һ��Ԫ������λ��
	if (!_final) *right = T_Seq(move, NULL);
	else *right = move;
}
//����Ĵ����ͳ�ʼ����ֱ�ӵ��ú��� initArray
Tr_exp Tr_arrayExp(Tr_exp _size, Tr_exp _val) {
	return Tr_Ex(F_externalCall("initArray", T_ExpList(unEx(_size), T_ExpList(unEx(_val), NULL))));
}
//ѭ���Ĵ���
//���Ƕ���Ƕ�� Label �Ĵ���ʹ��һ����ջ����ջ��ʵ�ֱȽϼ�
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
//Ȼ��������һ������ Label����������һ�� Label ����ջ��
void Tr_genLoopDoneLabel() {
	Temp_label l = Temp_newlabel();
	LL_push(l);
}
//while ѭ���Ĵ���
Tr_exp Tr_whileExp(Tr_exp _cond, Tr_exp _body) {
	T_stm st;
	//f ��Ӧ�� Label Ӧ���ں�������ʱ�Ѿ� Push ��ȥ
	Temp_label t = Temp_newlabel(), f = LL_peek();
	Temp_label start = Temp_newlabel();
	struct Cx cx_cond = unCx(_cond);
	doPatch(cx_cond.trues, t);
	doPatch(cx_cond.falses, f);
	//���ɶ�Ӧ���������������Ľṹ
	st = T_Seq(T_Label(start), T_Seq(cx_cond.stm, T_Seq(T_Label(t),	T_Seq(unNx(_body), T_Seq(T_Jump(T_Name(start),Temp_LabelList(start, NULL)),T_Label(f))))));
	//�� While ѭ������ʱ���������� Pop ������
	LL_pop();
	return Tr_Nx(st);
}
//for ѭ���Ĵ����� for ѭ��ת���� while ѭ������ʽ��
Tr_exp Tr_forExp(Tr_exp _var, Tr_exp _l, Tr_exp _h, Tr_exp _body) {
	T_stm st;
	T_exp v = unEx(_var);
	Temp_label t = Temp_newlabel(), f = LL_peek();
	Temp_label start = Temp_newlabel();
	//����һ�������� while ѭ����
	T_stm cond = T_Cjump(T_le, v, unEx(_h), t, f);
	st = T_Seq(T_Move(v, unEx(_l)), T_Seq(T_Label(start), T_Seq(cond, T_Seq(T_Label(t), T_Seq(unNx(_body), T_Seq(T_Move(v, T_Binop(T_plus, v, T_Const(1))), T_Seq(T_Jump(T_Name(start), Temp_LabelList(start, NULL)), T_Label(f))))))));
	//�� For ѭ������ʱ���������� Pop ������
	LL_pop();
	return Tr_Nx(st);
}
//break �Ĵ����ҵ���Ӧ�� done �ı�ǩ��Ȼ����ת
Tr_exp Tr_breakExp() {
	Temp_label f = LL_peek();
	T_stm st = T_Jump(T_Name(f), Temp_LabelList(f, NULL));
	return Tr_Nx(st);
}
//����������
Tr_exp Tr_callExp(Tr_level _callerLevel, Tr_level _calleeLevel, Temp_label _funLabel, Tr_exp* _argv, int _args) {
	int z = 0, cnt = 0;
	//���㾲̬��
	T_exp slk = F_Exp(F_staticLink(), T_Temp(F_FP()));;
	if (_callerLevel != _calleeLevel) {
		_callerLevel = _callerLevel->parent;
		while (_callerLevel != _calleeLevel) {
			slk = F_Exp(F_staticLink(), slk);
			_callerLevel = _callerLevel->parent;
		}
	}
	//��������б���������Ϊ0ʱ����̬��ͬ��Ӧ����Ϊһ������
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
	//���ɱ��ʽ
	T_exp exp = T_Call(T_Name(_funLabel), listp_head);
	return Tr_Ex(exp);
}
//����Ϊ�գ�ǰ��Ϊ�洢��Ϊ�գ��ڶ�����ֵΪ�գ�
Tr_exp Tr_nilExp() { return Tr_Ex(T_Mem(T_Const(0))); }
Tr_exp Tr_voidExp(void) { return Tr_Ex(T_Const(0)); }
//���ͳ����Ĵ���
Tr_exp Tr_intExp(int _val) { return Tr_Ex(T_Const(_val)); }
//��Ԫ�������
Tr_exp Tr_arithExp(A_oper _oper, Tr_exp _left, Tr_exp _right) {
	switch (_oper) {
	case A_plusOp: return Tr_Ex(T_Binop(T_plus, unEx(_left), unEx(_right)));
	case A_minusOp: return Tr_Ex(T_Binop(T_minus, unEx(_left), unEx(_right)));
	case A_timesOp:	return Tr_Ex(T_Binop(T_mul, unEx(_left), unEx(_right)));
	case A_divideOp: return Tr_Ex(T_Binop(T_div, unEx(_left), unEx(_right)));
	}
}
//��Ԫ�߼�����(������ַ����Ƚϣ��򵥶�����
Tr_exp Tr_logicExp(A_oper _op, Tr_exp _l, Tr_exp _r, bool _isStr) {
	T_stm stm;
	//������ַ����Ƚϣ������һ������
	if (_isStr) {
		T_exp call = F_externalCall("stringEqual", T_ExpList(unEx(_l), T_ExpList(unEx(_r), NULL)));
		if (_op == A_eqOp)stm = T_Cjump(T_eq, call, T_Const(1), NULL, NULL);
		else stm = T_Cjump(T_eq, call, T_Const(0), NULL, NULL);
	}
	//������ǣ���ֱ������ T_Cjump ��������
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
//��ֵ����
Tr_exp Tr_assignExp(Tr_exp _l, Tr_exp _r) {
	if (_r != NULL) return Tr_Nx(T_Move(unEx(_l), unEx(_r)));
	else return _l;
}
//�����¼����
Tr_exp Tr_fieldVar(Tr_exp _base, int _offset) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(_base), T_Const(_offset * F_wordSize))));
}
//��������
Tr_exp Tr_seqExp(Tr_exp* _array, int _size) {
	T_exp exp = (T_exp)checked_malloc(sizeof(*exp));
	T_exp *p = &exp, head;
	int i, last = _size - 1;
	for (i = 0; i < _size; i++) {
		//����������һ����Ҫ�����������λ��
		if (i != last) *p = T_Eseq(unNx(_array[i]), NULL);
		else *p = unEx(_array[i]);
		if (i == 0) head = *p;
		if (i != last) p = &((*p)->u.ESEQ.exp);
	}
	return Tr_Ex(head);
}
//��ȡ���յĽ��
T_stm getResult(struct expty _res){
	return unNx(_res.exp);
}



