/* tree.h
 * 中间表示树（IR Tree）的定义代码。
 * 主要对 IR 树的表示和构造所用到的一些结构和函数进行声明
 */

#ifndef TREE_H_
#define TREE_H_

#include "symbol.h"
#include "temp.h"
//定义各个类型的指针
typedef struct T_stm_ *T_stm;
typedef struct T_exp_ *T_exp;
typedef struct T_expList_ *T_expList;
typedef struct T_stmList_ *T_stmList;


//二元操作符的枚举类型
typedef enum {
	T_plus, T_minus, T_mul, T_div,
	T_and, T_or, T_lshift, T_rshift, T_arshift, T_xor
} T_binOp;
typedef enum {
	T_eq, T_ne, T_lt, T_gt, T_le, T_ge,
	T_ult, T_ule, T_ugt, T_uge
} T_relOp;

//表达式的结构体定义，共有七种类型
struct T_exp_ {
	enum {
		T_CONST, T_NAME, T_TEMP, T_BINOP, 
		T_MEM, T_CALL, T_ESEQ
	} kind;
	union {
		int CONST;
		Temp_label NAME;
		Temp_temp TEMP;
		struct { T_binOp op; T_exp left, right; } BINOP;
		T_exp MEM;
		struct { T_exp fun; T_expList args; } CALL;
		struct { T_stm stm; T_exp exp; } ESEQ;
	} u;
};
//构造表达式的函数
T_exp T_Const(int);
T_exp T_Name(Temp_label);
T_exp T_Temp(Temp_temp);
T_exp T_Binop(T_binOp, T_exp, T_exp);
T_exp T_Mem(T_exp);
T_exp T_Call(T_exp, T_expList);
T_exp T_Eseq(T_stm, T_exp);

//语句的结构体定义，共有六种类型。
struct T_stm_ {
	enum {
		T_MOVE, T_EXP, T_JUMP, T_CJUMP,
		T_SEQ, T_LABEL		
	} kind;
	union {
		struct { T_exp dst, src; } MOVE;
		T_exp EXP;
		struct { T_exp exp; Temp_labelList jumps; } JUMP;
		struct {
			T_relOp op; T_exp left, right;
			Temp_label true, false;
		} CJUMP;
		struct { T_stm left, right; } SEQ;
		Temp_label LABEL;	
	} u;
};
//构造语句的函数
T_stm T_Move(T_exp, T_exp);
T_stm T_Exp(T_exp);
T_stm T_Jump(T_exp, Temp_labelList);
T_stm T_Cjump(T_relOp, T_exp, T_exp, Temp_label, Temp_label);
T_stm T_Seq(T_stm, T_stm);
T_stm T_Label(Temp_label);

//语句和表达式的 List 类型
struct T_stmList_ { T_stm head; T_stmList tail; };
struct T_expList_ { T_exp head; T_expList tail; };
//构造 List 的函数
T_expList T_ExpList(T_exp, T_expList);
T_stmList T_StmList(T_stm, T_stmList);

T_relOp T_notRel(T_relOp);  /* a op b    ==     not(a notRel(op) b)  */
T_relOp T_commute(T_relOp); /* a op b    ==    b commute(op) a       */

#endif // enf ifndef TREE_H_