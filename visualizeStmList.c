#include "visualizeStmList.h"
#include "absyn.h"
#include "tree.h"
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include <string.h>
#include <stdio.h>

static char bin_oper[][12] = {
	"PLUS", "MINUS", "TIMES", "DIVIDE",
	"AND", "OR", "LSHIFT", "RSHIFT", "ARSHIFT", "XOR" };
static char rel_oper[][12] = {
	"EQ", "NE", "LT", "GT", "LE", "GE", "ULT", "ULE", "UGT", "UGE" };
static void init(FILE *out) {
	fprintf(out, "digraph {\n");
}
static void finish(FILE *out) {
	fprintf(out, "}\n");
}
static void inline genLabel(FILE *out, int node, string label) {
	fprintf(out, "%d [label=\"%s\"];\n", node, label);
}
static void inline pr_edge(FILE *out, int parent, int child) {
	fprintf(out, "%d -> %d;\n", parent, child);
}

// The given node id
static int node_id = 0;


static void vis_tree_exp(FILE *out, T_exp exp,int parent);
static void vis_stm(FILE *out, T_stm stm, int parent) {
	int cur, c1, c2, c3, c4, c5;
	switch (stm->kind) {
	case T_SEQ:
		cur = ++node_id;
		genLabel(out, cur, "SEQ");
		fprintf(out, "%d -> %d;\n", parent, cur);
		vis_stm(out, stm->u.SEQ.left, cur);
		vis_stm(out, stm->u.SEQ.right, cur);
		break;
	case T_LABEL:
		cur = ++node_id;
		genLabel(out, cur, "LABEL");
		fprintf(out, "%d -> %d;\n", parent, cur);
		c1 = ++node_id;
		genLabel(out, c1, S_name(stm->u.LABEL));
		fprintf(out, "%d -> %d;\n", cur, c1);
		break;
	case T_JUMP:
		cur = ++node_id;
		genLabel(out, cur, "JUMP");
		pr_edge(out, parent, cur);
		vis_tree_exp(out, stm->u.JUMP.exp, cur);
		break;
	case T_CJUMP:
		cur = ++node_id;
		genLabel(out, cur, "CJUMP");
		pr_edge(out, parent, cur);
		// need to print child
		// 1) print jump condition
		c1 = ++node_id;
		genLabel(out, c1, rel_oper[stm->u.CJUMP.op]);
		// 2) print the left and right exp
		vis_tree_exp(out, stm->u.CJUMP.left, cur);
		vis_tree_exp(out, stm->u.CJUMP.right, cur);
		// 3) print the true and false label
		c2 = ++node_id;
		c3 = ++node_id;
		genLabel(out, c2, S_name(stm->u.CJUMP.true));
		genLabel(out, c3, S_name(stm->u.CJUMP.false));
		pr_edge(out, cur, c2);
		pr_edge(out, cur, c3);
		break;
	case T_MOVE:
		cur = ++node_id;
		genLabel(out, cur, "MOVE");
		pr_edge(out, parent, cur);
		vis_tree_exp(out, stm->u.MOVE.dst, cur);
		vis_tree_exp(out, stm->u.MOVE.src, cur);
		break;
	case T_EXP:
		// we don't need to add a new node
		vis_tree_exp(out, stm->u.EXP, parent);
		break;
	}
}

static void vis_tree_exp(FILE *out, T_exp exp, int parent) {
	int cur, c1, c2, c3;
	switch (exp->kind)	{
	case T_BINOP :
		cur = ++node_id;
		genLabel(out, cur, "BINOP");
		pr_edge(out, parent, cur);
		// generate the binary operation
		c1 = ++node_id;
		genLabel(out, c1, bin_oper[exp->u.BINOP.op]);
		pr_edge(out, cur, c1);
		vis_tree_exp(out, exp->u.BINOP.left, cur);
		vis_tree_exp(out, exp->u.BINOP.right, cur);
		break;
	case T_MEM:
		cur = ++node_id;
		genLabel(out, cur, "MEM");
		pr_edge(out, parent, cur);
		vis_tree_exp(out, exp->u.MEM, cur);
		break;
	case T_TEMP :
		cur = ++node_id;
		genLabel(out, cur, "TEMP");
		pr_edge(out, parent, cur);
		c1 = ++node_id;
		genLabel(out, c1, Temp_look(Temp_name(), exp->u.TEMP));
		pr_edge(out, cur, c1);
		break;
	case T_ESEQ :
		cur = ++node_id;
		genLabel(out, cur, "ESEQ");
		pr_edge(out, parent, cur);
		vis_stm(out, exp->u.ESEQ.stm, cur);
		vis_tree_exp(out, exp->u.ESEQ.exp, cur);
		break;
	case T_NAME :
		cur = ++node_id;
		genLabel(out, cur, "NAME");
		pr_edge(out, parent, cur);
		c1 = ++node_id;
		genLabel(out, c1, S_name(exp->u.NAME));
		pr_edge(out, cur, c1);
		break;
	case T_CONST :
		cur = ++node_id;
		char buf[80];
		strcpy(buf, "CONST  ");
		itoa(exp->u.CONST, buf+6, 10);
		// const number is in the label
		genLabel(out, cur, buf);
		pr_edge(out, parent, cur);
		break;
	case T_CALL:
		cur = ++node_id;
		genLabel(out, cur, "CALL");
		pr_edge(out, parent, cur);
		vis_tree_exp(out, exp->u.CALL.fun, cur);
		T_expList args = exp->u.CALL.args;

		for (; args;args = args->tail) {
			vis_tree_exp(out, args->head, cur);
		}
		break;
	}
}
void visualStmList(FILE * out, T_stmList stmList)
{
	init(out);
	for (; stmList;stmList = stmList->tail) {
		vis_stm(out, stmList->head, 0);
	}
	finish(out);
}
