%{
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "absyn.h"
#include "util.h"
#include "frame.tab.h"
#include "errormsg.h"
#define MAX_STR_CONST 256
int charPos = 1;
int yywrap(void){
	charPos = 1;
	return 1;
}

void adjust(void){
	EM_tokPos = charPos;
	charPos += yyleng;
}

/* Things to deal with string */
char string_buf[MAX_STR_CONST];
char *string_buf_ptr;

/* Things to deal with comments */
int comm_level = 0;

%}

int_val [0-9]+
id [A-Za-z][_a-zA-Z0-9]*

%x str
%x comments

%%
\"  { adjust(); string_buf_ptr = string_buf; BEGIN(str);}
"/*"	{ adjust(); comm_level++; BEGIN(comments); }
[ \t]+ 	{ adjust(); }
[\n] 	{ adjust(); EM_newline();}
"," 	{ adjust(); return COMMA;}
":="	{ adjust(); return ASSIGN;}
";" 	{ adjust(); return SEMICOLON; }
":"	{ adjust(); return COLON;}
"(" 	{ adjust(); return LPAREN; }
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
<str>\" { /* saw closing quote - all done */
        adjust();
		BEGIN(INITIAL);
        *string_buf_ptr = '\0';
        yylval.sval = String(string_buf);
        return STRING;
        }
<str>\n        {
		adjust();EM_newline();
		*string_buf_ptr = '\0';
		EM_error(EM_tokPos,"multiline string is not support:%s",string_buf);
        /* error - unterminated string constant */
        /* generate error message */
        }

<str>\\[0-7]{1,3} {
		adjust();
        /* octal escape sequence */
        int result;

        (void) sscanf( yytext + 1, "%o", &result );

        if ( result > 0xff )
                /* error, constant is out-of-bounds */

        *string_buf_ptr++ = result;
        }

<str>\\[0-9]+ {
		adjust();
        /* generate error - bad escape sequence; something
         * like '\48' or '\0777777'
         */
		EM_error(EM_tokPos,"bad escape sequence:%s",yytext);
        }

<str>\\n  { adjust(); *string_buf_ptr++ = '\n'; }
<str>\\t  { adjust(); *string_buf_ptr++ = '\t'; }
<str>\\r  { adjust(); *string_buf_ptr++ = '\r'; }
<str>\\b  { adjust(); *string_buf_ptr++ = '\b'; }
<str>\\f  { adjust(); *string_buf_ptr++ = '\f'; }

<str>\\(.|\n)  { adjust(); *string_buf_ptr++ = yytext[1];}

<str>[^\\\n\"]+        {
		adjust(); 
        char *yptr = yytext;

        while ( *yptr )
                *string_buf_ptr++ = *yptr++;
        }

<comments>{
	"/*"	{ adjust(); comm_level ++; }
	"*/"	{ adjust(); comm_level--; if(comm_level==0) BEGIN(INITIAL); }
	"\n"    { adjust(); EM_newline(); }	
	.	/* Do nothing */ adjust();
}
. { /*Log("Lex analyse","Unknown token:%s",yytext);*/ EM_error(EM_tokPos,"Unknown token:%s",yytext); }

%%
