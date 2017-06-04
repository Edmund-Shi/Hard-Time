/* translate.h
 * �м�������ɵĹؼ��ļ�����Ҫ�����ǽ������﷨��ת���� IR Tree��
 * ��Ҫ�Ĺ����Ǵ���ܶ��ָ��ӵ��������һЩ�ؼ��ĺ�������������
 */
#ifndef TRANSLATE_H_
#define TRANSLATE_H_
#include "util.h"
#include "tree.h"
#include "temp.h"
#include "frame.h"
#include "absyn.h" 

//��Ż�����е������͹���
typedef struct patchList_ *patchList;
struct patchList_ {
	Temp_label *head;
	patchList tail;
};
static patchList PatchList(Temp_label, patchList);
//����ת�Ʊ��ʽ�Ľṹ
struct Cx {
	patchList trues, falses;
	T_stm stm;
};
//������ʾ�����﷨���еı��ʽ
typedef struct Tr_exp_ *Tr_exp;
struct Tr_exp_ {
	enum { Tr_ex, Tr_nx, Tr_cx }kind;
	union {
		T_exp ex;
		T_stm nx;
		struct Cx cx;
	} u;
};
//�������캯��
static Tr_exp Tr_Ex(T_exp);
static Tr_exp Tr_Nx(T_stm);
static Tr_exp Tr_Cx(patchList, patchList, T_stm);

//һ����������������ת������
void doPatch(patchList, Temp_label);
static T_exp unEx(Tr_exp);
static T_stm unNx(Tr_exp);
static struct Cx unCx(Tr_exp);
//���ڱ����洢��ջ֡������һЩ�ṹ����
typedef struct Tr_level_ *Tr_level;
struct Tr_level_ {
	Tr_level parent;
	F_frame frame;
	int depth; // call stack depth
};
typedef struct Tr_access_ *Tr_access;
struct Tr_access_ {
	Tr_level level;
	F_access access;
};
typedef struct Tr_accessList_ *Tr_accessList;
struct Tr_accessList_ {
	Tr_access head;
	Tr_accessList tail;
};
//�����洢��ջ֡�����ĺ�������
//��������Ĳ㼶�ṹ
Tr_level Tr_outLevel();
//����һ���µĲ㼶
Tr_level Tr_newLevel(Tr_level, Temp_label, U_boolList);
//���ݲ㼶�õ��ò���ԽӴ������б�������
Tr_accessList Tr_formals(Tr_level);
//�õ�ĳһ�������ľ���Ӵ���Ϣ
Tr_access Tr_allocLocal(Tr_level, bool);
//�õ�һ���㼶�ĸ���
Tr_level Tr_getParent(Tr_level);
//�򵥱������������
Tr_exp Tr_simpleVar(Tr_access, Tr_level);
Tr_exp Tr_arrayVar(Tr_exp, Tr_exp);
//���� if ���Ĵ���
Tr_exp Tr_ifExp(Tr_exp, Tr_exp, Tr_exp);
//�����ַ����Ĵ���
Tr_exp Tr_stringExp(string);
//���ڼ�¼�����Ĵ����ڶ����Ǵ���ÿһ��Ԫ��
Tr_exp Tr_recordExp_new(int);
void Tr_recordExp_app(Tr_exp, Tr_exp, bool);
//����Ĵ����Լ���ʼ��
Tr_exp Tr_arrayExp(Tr_exp, Tr_exp);
//ѭ���Ĵ���
void Tr_genLoopDoneLabel();
Tr_exp Tr_whileExp(Tr_exp, Tr_exp);
Tr_exp Tr_forExp(Tr_exp, Tr_exp, Tr_exp, Tr_exp);
Tr_exp Tr_breakExp();
//��������
Tr_exp Tr_callExp(Tr_level, Tr_level, Temp_label , Tr_exp*, int);
//����Ϊ�գ�ǰ��Ϊ�洢��Ϊ�գ��ڶ�����ֵΪ�գ�
Tr_exp Tr_nilExp();
Tr_exp Tr_voidExp(void);
//���ͳ����Ĵ���
Tr_exp Tr_intExp(int);
//��Ԫ�������
Tr_exp Tr_arithExp(A_oper, Tr_exp, Tr_exp);
//��Ԫ�߼�����
Tr_exp Tr_logicExp(A_oper, Tr_exp, Tr_exp, bool);
//��ֵ����
Tr_exp Tr_assignExp(Tr_exp, Tr_exp);
//�����¼����
Tr_exp Tr_fieldVar(Tr_exp, int);
//��������
Tr_exp Tr_seqExp(Tr_exp*, int);
//��ȡ���յĽ��
T_stm getResult(struct expty);
#endif
