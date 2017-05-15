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

/* Need to add token to deal with comments and string !*/
%}

int_val [0-9]+
id [A-Za-z][_a-zA-Z0-9]*


%%
[ \t]+ 	{ adjust(); }
[\n] 	{ adjust(); EM_newline();}
"," 	{ adjust(); return COMMA;}
":="	{ adjust(); return ASSIGN;}
";" 	{ adjust(); return SEMICOLON; }
":"	{ adjust(); return COLON;}
"(" 	{ adjust(); return LPAREN;}
")"	{ adjust(); return RPAREN;}
"{"	{ adjust(); return LBRACE; }
"}"	{ adjust(); return RBRACE; }
"["	{ adjust(); return LBRACK; }
"]"	{ adjust(); return RBRACK; }
"." 	{ adjust(); return DOT; }
"+"	{ adjust(); return PLUS; }
"-"	{ adjust(); return MINUS; }
"*"	{ adjust(); return TIMES;}
"/"	{ adjust(); return DIVIDE; }
"="	{ adjust(); return EQ;}
"<>"	{ adjust(); return NEQ;}
"<=" 	{ adjust(); return LE; }
"<" 	{ adjust(); return LT;}
">=" 	{ adjust(); return GE;}
">"	{ adjust(); return GT;}
"&"	{ adjust(); return AND;}
"|"	{ adjust(); return OR;}
array	{ adjust(); return ARRAY;}
break	{ adjust(); return BREAK;}
do 	{ adjust(); return DO;}
end	{ adjust(); return END;}
else 	{ adjust(); return ELSE;}
function {adjust(); return FUNCTION;}
for	{ adjust(); return FOR;}
if	{ adjust(); return IF;}
in	{ adjust(); return IN;}
let     { adjust(); return LET;}
of      { adjust(); return OF;}
nil     { adjust(); return NIL;}
then    { adjust(); return THEN;}
to 	{ adjust(); return TO;}
type	{ adjust(); return TYPE;}
var 	{ adjust(); return VAR;}
while   { adjust(); return WHILE;}
{id}	{ 
	adjust(); 
	yylval.sval = String(yytext);
	return ID;
}
{int_val} { 
	adjust();
	yylval.ival = atoi(yytext); 
	return INTT; 
}

. { logRunningInfo(yytext); }

%%
