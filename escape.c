#include "escape.h"

static int depth;
static TAB_table environment;

void traverseDec(A_dec decl);
void traverseExp(A_exp expr);
void traverseVar(A_var var);

typedef struct E_entry_ *E_entry;
struct E_entry_{
	int depth;
	bool *escape;
};

static E_entry e_entry(int depth, bool *escape){
	assert(escape);
	E_entry tmp = checked_malloc(sizeof(*tmp));
	tmp->depth = depth;
	tmp->escape = escape;
	*escape = FALSE;
	return tmp;
}

void traverseVar(A_var var) {
	//A_simpleVar does noting
	if (var->kind == A_fieldVar) {
		traverseVar(var->u.field.var);
	}
	else if (var->kind == A_subscriptVar) {
		traverseVar(var->u.subscript.var);
		traverseExp(var->u.subscript.exp);
	}

}

void traverseDec(A_dec dec) {
	A_fundecList list1;
	A_fieldList list2;

	//A_typeDec do nothing

	if (dec->kind == A_functionDec) {
		//暂且把所有元素当作是逃逸的
		list1 = dec->u.function;
		while (list1 != NULL) {
			depth++;
			S_beginScope(environment);
			list2 = list1->head->params;
			while (list2 != NULL) {
				S_enter(environment, list2->head->name, e_entry(depth, &list2->head->escape));
				list2 = list2->tail;
			}
			traverseExp(list1->head->body);
			S_endScope(environment);
			depth--;
			list1 = list1->tail;
		}
	}
	else if (dec->kind == A_varDec) {
		S_enter(environment, dec->u.var.var, e_entry(depth, &dec->u.var.escape));
		if (dec->u.var.init->kind == A_arrayExp || dec->u.var.init->kind == A_stringExp)
			dec->u.var.escape = TRUE;
		traverseExp(dec->u.var.init);

	}
	
}

void traverseExp(A_exp exp) {
	A_expList list1,list3;
	A_efieldList list2;
	A_decList list4;
	
	//A_nilExp  A_intExp A_stringExp A_breakExp do noting
	if (exp->kind == A_varExp) {
		traverseExp(exp->u.var);
	}
	else if (exp->kind == A_callExp) {
		list1 = exp->u.call.args;
		while (list1 != NULL) {
			traverseExp(list1->head);
			list1 = list1->tail;
		}
	}
	else if (exp->kind == A_opExp) {
		traverseExp(exp->u.op.left);
		traverseExp(exp->u.op.right);
	}
	else if (exp->kind == A_recordExp) {
		list2 = exp->u.record.fields;
		while (list2 != NULL) {
			traverseExp(list2->head->exp);
			list2 = list2->tail;
		}
	}	
	else if (exp->kind == A_seqExp) {
		list3 = exp->u.seq;
		while (list3 != NULL) {
			traverseExp(list3->head);
			list3 = list3->tail;
		}
	}
	else if (exp->kind == A_assignExp) {
		traverseVar(exp->u.assign.var);
		traverseExp(exp->u.assign.exp);
	}
	else if (exp->kind == A_ifExp) {
		traverseExp(exp->u.iff.test);
		traverseExp(exp->u.iff.then);
		if (exp->u.iff.elsee != NULL) {
			traverseExp(exp->u.iff.elsee);
		}
	}
	else if (exp->kind == A_whileExp) {
		traverseExp(exp->u.whilee.test);
		traverseExp(exp->u.whilee.body);
	}
	else if (exp->kind == A_forExp) {
		traverseExp(exp->u.forr.lo);
		traverseExp(exp->u.forr.hi);
		S_beginScope(environment);
		S_enter(environment, exp->u.forr.var, e_entry(depth, &exp->u.forr.escape));
		traverseExp(exp->u.forr.body);
		S_endScope(environment);
	}
	else if (exp->kind == A_letExp) {
		S_beginScope(environment);
		list4 = exp->u.let.decs;
		while (list4 != NULL) {
			traverseDec(list4->head);
			list4 = list4->tail;
		}
		traverseExp(exp->u.let.body);
		S_endScope(environment);
	}
	else if (exp->kind == A_arrayExp) {
		traverseExp(exp->u.array.size);
		traverseExp(exp->u.array.init);
		
	}
}


void Esc_findEscape(A_exp exp) {
	depth = 0;
	environment = S_empty();
	traverseExp(exp);
}
