/* translate.h
 * 中间代码生成的关键文件，主要作用是将抽象语法树转换成 IR Tree。
 * 主要的功能是处理很多种复杂的情况。对一些关键的函数进行声明。
 */
#ifndef TRANSLATE_H_
#define TRANSLATE_H_
#include "util.h"
#include "tree.h"
#include "temp.h"
#include "frame.h"
#include "absyn.h" 

//标号回填队列的声明和构造
typedef struct patchList_ *patchList;
struct patchList_ {
	Temp_label *head;
	patchList tail;
};
static patchList PatchList(Temp_label, patchList);
//条件转移表达式的结构
struct Cx {
	patchList trues, falses;
	T_stm stm;
};
//用来表示抽象语法树中的表达式
typedef struct Tr_exp_ *Tr_exp;
struct Tr_exp_ {
	enum { Tr_ex, Tr_nx, Tr_cx }kind;
	union {
		T_exp ex;
		T_stm nx;
		struct Cx cx;
	} u;
};
//三个构造函数
static Tr_exp Tr_Ex(T_exp);
static Tr_exp Tr_Nx(T_stm);
static Tr_exp Tr_Cx(patchList, patchList, T_stm);

//一个辅助函数和三个转换函数
void doPatch(patchList, Temp_label);
static T_exp unEx(Tr_exp);
static T_stm unNx(Tr_exp);
static struct Cx unCx(Tr_exp);
//对于变量存储和栈帧操作的一些结构定义
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
//变量存储和栈帧操作的函数定义
//获得最外层的层级结构
Tr_level Tr_outLevel();
//创造一个新的层级
Tr_level Tr_newLevel(Tr_level, Temp_label, U_boolList);
//根据层级得到该层可以接触的所有变量链表
Tr_accessList Tr_formals(Tr_level);
//得到某一个变量的具体接触信息
Tr_access Tr_allocLocal(Tr_level, bool);
//得到一个层级的父层
Tr_level Tr_getParent(Tr_level);
//简单变量和数组变量
Tr_exp Tr_simpleVar(Tr_access, Tr_level);
Tr_exp Tr_arrayVar(Tr_exp, Tr_exp);
//对于 if 语句的处理。
Tr_exp Tr_ifExp(Tr_exp, Tr_exp, Tr_exp);
//对于字符串的处理
Tr_exp Tr_stringExp(string);
//对于记录创建的处理，第二个是处理每一个元素
Tr_exp Tr_recordExp_new(int);
void Tr_recordExp_app(Tr_exp, Tr_exp, bool);
//数组的创建以及初始化
Tr_exp Tr_arrayExp(Tr_exp, Tr_exp);
//循环的处理
void Tr_genLoopDoneLabel();
Tr_exp Tr_whileExp(Tr_exp, Tr_exp);
Tr_exp Tr_forExp(Tr_exp, Tr_exp, Tr_exp, Tr_exp);
Tr_exp Tr_breakExp();
//函数调用
Tr_exp Tr_callExp(Tr_level, Tr_level, Temp_label , Tr_exp*, int);
//声明为空（前者为存储器为空，第二个是值为空）
Tr_exp Tr_nilExp();
Tr_exp Tr_voidExp(void);
//整型常量的处理
Tr_exp Tr_intExp(int);
//二元计算操作
Tr_exp Tr_arithExp(A_oper, Tr_exp, Tr_exp);
//二元逻辑操作
Tr_exp Tr_logicExp(A_oper, Tr_exp, Tr_exp, bool);
//赋值操作
Tr_exp Tr_assignExp(Tr_exp, Tr_exp);
//定义记录变量
Tr_exp Tr_fieldVar(Tr_exp, int);
//多个语句拆分
Tr_exp Tr_seqExp(Tr_exp*, int);
//获取最终的结果
T_stm getResult(struct expty);
#endif
