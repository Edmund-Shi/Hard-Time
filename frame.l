%{
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "absyn.h"
#include "util.h"
#include "frame.tab.h"
#include "errormsg.h"

int charPos = 1;

int yywrap(void){
	charPos = 1;
	return 1;
}

void adjust(void){
	EM_tokPos = charPos;
	charPos += yyleng;
}

%}

int_val [0-9]+

%%

let { return LET; }
in { return IN; }
end { return END; }

{int_val} { 
	yylval.iVal = atoi(yytext); 
	return INTT; 
}

[ \t\r] { }
[\n] {EM_newline();}
. { logRunningInfo(yytext); }

%%
