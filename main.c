#include <stdio.h>
#include "util.h"
#include "absyn.h"
#include "frame.tab.h"
#include "log.h"
#include "semant.h"

extern int yyparse();
A_exp root;
int main(void){
	ClearLog();
	yyparse();
	SEM_transProg(root);

	printf("Parse Done\n");
	return 0;
}