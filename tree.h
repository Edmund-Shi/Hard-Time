/* 
* Definitions for intermediate representation (IR) trees.
*/
#ifndef TREE_H_
#define TREE_H_

// mang typedefs
typedef struct T_stm_ *T_stm;
typedef struct T_exp_ *T_exp;
typedef struct T_expList_ *T_expList;
typedef struct T_stmList_ *T_stmList;

// struct definitions
struct T_expList_ {T_exp head; T_expList tail;};
struct T_stmList_ {T_stm head; T_stmList tail;};

// TODO 需要根据新增加的语句来扩展以下的内容
struct T_stm_{
	enum {T_SEQ} kind;
	union {
		struct { T_stm left, right; } SEQ;
	} u ;
};

// TODO 需要添加新的内容
struct T_exp_{
	enum {T_MEM, T_CONST} kind;
	union {
		T_exp MEM;
		int CONST;
	} u ;
};

// function declaration
T_expList T_ExpList(T_exp head, T_expList tail);
T_stmList T_StmList(T_stm head, T_stmList tail);

T_exp T_Const(int intt);

#endif // #ifndef TREE_H_