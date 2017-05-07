%{
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "absyn.h"
#include "util.h"
#include "frame.tab.h"
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

[ \t\r\n] { }
. { logRunningInfo(yytext); }

%%

int yywrap(void){
	return 1;
}
