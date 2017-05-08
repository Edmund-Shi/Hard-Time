#include "tree.h"
#include "util.h"

T_expList T_ExpList(T_exp head, T_expList tail);
T_stmList T_StmList(T_stm head, T_stmList tail);

T_exp T_Const(int intt){
	T_exp p = (T_exp)checked_malloc(sizeof(*p));
	p->kind = T_CONST;
	p->u.CONST = intt;
	return p;
}
