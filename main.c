#include <stdio.h>
#include "util.h"
#include "absyn.h"
#include "frame.tab.h"
#include "log.h"
#include "semant.h"
#include "prabsyn.h"

extern int yyparse();
A_exp root;
int main(void){
	FILE *absynTree;
	absynTree = fopen("AbsynTree.txt", "w");
	if (absynTree == NULL){
		exit(0);
	}
	ClearLog();
	yyparse();
	//SEM_transProg(root);

	pr_exp(absynTree, root, 0);
	fclose(absynTree);
	printf("Parse Done\n");
	return 0;
}