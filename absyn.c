#include "absyn.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
A_exp A_IntExp(int i){
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->kind = A_INT_EXP;
	p->u.intt = i;
	return p;
}
A_exp A_LetExp(A_exp body){
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->kind = A_LET_EXP;
	p->u.let.body = body;
	return p;
}