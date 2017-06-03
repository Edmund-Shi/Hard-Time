#include <stdio.h>
#include "util.h"
#include "absyn.h"
#include "frame.tab.h"
#include "log.h"
#include "semant.h"
#include "prabsyn.h"
#include "visualizeStmList.h"

extern int yyparse();
A_exp root;
int main(void){
	FILE *absynTree, *test,*IRtree,*IRGraph;
	absynTree = fopen("AbsynTree.txt", "w");
	IRtree = fopen("IRtree.txt", "w");
	IRGraph = fopen("IRGraph.dot", "w");
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
	visualStmList(IRGraph, list);

	fclose(absynTree);
	fclose(IRtree);
	fclose(IRGraph);

	printf("Parse Done\n");
	closeLog();
	return 0;
}