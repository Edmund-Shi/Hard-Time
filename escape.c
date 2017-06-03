#include "escape.h"
#include "table.h"
#include "symbol.h"
static int depth;
static TAB_table env;


static void traverseDec(A_dec decl);
static void traverseExp(A_exp expr);
static void traverseVar(A_var var);

typedef struct escape_entry_s *escape_entry_t;
struct escape_entry_s
{
	int depth;
	bool *escape;
};

static escape_entry_t escape_entry(int depth, bool *escape)
{
	assert(escape);
	escape_entry_t p = checked_malloc(sizeof(*p));
	p->depth = depth;
	p->escape = escape;
	*escape = FALSE;
	return p;
}

void traverseVar(A_var var) {
	switch (var->kind) {
	case A_simpleVar:
		break;
	case A_fieldVar:
		traverseVar(var->u.field.var);
		break;
	case A_subscriptVar:
		traverseVar(var->u.subscript.var);
		traverseExp(var->u.subscript.exp);
		break;
	}		
}

void traverseDec(A_dec dec) {
	A_fundecList list1;
	A_fieldList list2;
	switch (dec->kind) {
	case A_functionDec:
		list1 = dec->u.function;
		while (list1 != NULL) {
			depth++;
			S_beginScope(env);
			list2 = list1->head->params;
			while (list2 != NULL) {
				S_enter(env,list2->head->name,escape_entry(depth, &list2->head->escape));
				
				list2 = list2->tail;
			}
			traverseExp(list1->head->body);
			S_endScope(env);
			depth--;
			list1 = list1->tail;
		}
		break;
	case A_varDec:
		break;
	case A_typeDec:
		S_enter(env, dec->u.var.var, escape_entry(depth, &dec->u.var.escape));
		traverseExp(dec->u.var.init);
		break;

	}
}

void traverseExp(A_exp exp) {
	A_expList list1,list3;
	A_efieldList list2;
	A_decList list4;
	switch (exp->kind){
	case A_varExp:
		traverseExp(exp->u.var);
		break;
	case A_nilExp:
		break;
	case A_intExp: 
		break;
	case A_stringExp:
		break;
	case A_callExp:
		list1 = exp->u.call.args;
		while (list1 != NULL) {
			traverseExp(list1->head);
			list1 = list1->tail;
		}
		break;
	case A_opExp:
		traverseExp(exp->u.op.left);
		traverseExp(exp->u.op.right);
		break;
	case A_recordExp:
		list2 = exp->u.record.fields;
		while (list2 != NULL) {
			traverseExp(list2->head->exp);
			list2 = list2->tail;
		}
		break;
	case A_seqExp:
		list3 = exp->u.seq;
		while (list3 != NULL) {
			traverseExp(list3->head);
			list3 = list3->tail;
		}
		break;
	case A_assignExp:
		traverseVar(exp->u.assign.var);
		traverseExp(exp->u.assign.exp); 
		break;
	case A_ifExp:
		traverseExp(exp->u.iff.test);
		traverseExp(exp->u.iff.then);
		if (exp->u.iff.elsee != NULL) {
			traverseExp(exp->u.iff.elsee);
		}
		break;
	case A_whileExp:
		traverseExp(exp->u.whilee.test);
		traverseExp(exp->u.whilee.body);
		break;
	case A_forExp:
		traverseExp(exp->u.forr.lo);
		traverseExp(exp->u.forr.hi);
		S_beginScope(env);
		S_enter(env, exp->u.forr.var, escape_entry(depth, &exp->u.forr.escape));
		traverseExp(exp->u.forr.body);
		S_endScope(env);
		break;
	case A_breakExp:
		break;
	case A_letExp:
		S_beginScope(env);
		list4 = exp->u.let.decs;
		while (list4 != NULL) {
			traverseDec(list4->head);
			list4 = list4->tail;
		}
		traverseExp(exp->u.let.body);
		S_endScope(env);
		break;
	case A_arrayExp:
		traverseExp(exp->u.array.size);
		traverseExp(exp->u.array.init);
		break;
	}
}
void Esc_findEscape(A_exp exp) {
	depth = 0;
	env = S_empty();
	traverseExp(exp);
}
