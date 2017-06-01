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
	FILE *absynTree, *test;
	absynTree = fopen("AbsynTree.txt", "w");
	freopen("test.txt", "r", stdin);
	if (absynTree == NULL){
		printf("Can't open abtree file!\n");
		exit(0);
	}
	ClearLog();
	openLog(); /*open log at the beginnig and colse it before exit*/
	yyparse();
	SEM_transProg(root);

	pr_exp(absynTree, root, 0);
	fclose(absynTree);
	printf("Parse Done\n");
	closeLog();
	return 0;
}