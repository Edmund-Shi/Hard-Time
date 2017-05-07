#include <stdio.h>
#include "util.h"
#include "absyn.h"
#include "frame.tab.h"
extern int yyparse();
A_exp root;
int main(void){
	yyparse();
	printf("Parse Done\n");
	return 0;
}