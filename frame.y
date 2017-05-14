%{
	#include <stdio.h>
	#include <string.h>
	#include "log.h"
	#include "util.h"
	#include "absyn.h"
	#include "errormsg.h"
	extern int yylex(void);
	extern A_exp root;
%}

%union {
	int iVal;
	A_exp expVal;
}
%start ProgramRoot
%token <iVal> INTT

%token LET IN END

%type <expVal> Exp ProgramRoot

%%
ProgramRoot:
	Exp { 
		logRunningInfo("Parse Exp Begin");
		root = $1;
		logRunningInfo("Parse Exp End");
	}
	;
Exp:
	LET IN Exp END { $$ = A_LetExp(EM_tokPos,NULL,$3); }
	|INTT { $$ = A_IntExp(EM_tokPos,yylval.iVal); }
	;

%%
yyerror(char *s){
	return 0;
}

