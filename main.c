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
	FILE *absynTree, *test,*IRtree;
	absynTree = fopen("AbsynTree.txt", "w");
	IRtree = fopen("IRtree.txt", "w");
	freopen("test.txt", "r", stdin);
	if (absynTree == NULL){
		printf("Can't open abtree file!\n");
		exit(0);
	}
	ClearLog();
	openLog(); /*open log at the beginnig and colse it before exit*/
	yyparse();
	T_stm result;
 	result = SEM_transProg(root);
	T_stmList list = T_StmList(result, NULL);
	printStmList(IRtree, list);

	pr_exp(absynTree, root, 0);
	fclose(absynTree);
	fclose(IRtree);
	printf("Parse Done\n");
	closeLog();
	return 0;
}