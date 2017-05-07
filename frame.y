%{
	#include <stdio.h>
	#include <string.h>
	#include "log.h"
	#include "util.h"
	#include "absyn.h"
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
	LET IN Exp END { $$ = A_LetExp($3); }
	|INTT { $$ = A_IntExp(yylval.iVal); }
	;

%%
yyerror(char *s){
	
}

