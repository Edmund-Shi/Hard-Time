%{
	#include <stdio.h>
	#include <string.h>
	#include "log.h"
	#include "util.h"
	#include "absyn.h"
	#include "errormsg.h"
	extern int yylex(void);
	extern A_exp root;

#define LVALUE_ACTION(target,prev,elem) \
	do \
	{ \
		A_var p, var = (elem); \
		(target) = p = (prev); \
		if (p) \
		{ \
			while( p -> u.field.var) \
				p = p -> u.field.var; \
			p -> u.field.var = var;\
		}\
		else \
			(target) = var;\
	}\
	while(0)

%}

%union {
	int pos;
	int ival;
	string sval;
	A_exp exp;
	S_symbol sym;
	A_var var;
	A_expList explist;
	A_ty type_p;
	A_namety name_type;
	A_dec decl;
	A_fieldList flist;
	A_fundecList fdeclist;
	A_decList dec_List;
	A_efieldList e_list;
}
%token <sval> ID
%token <ival> INTT
%token <sval> STRING

%type <exp> exp
%type <sym> id
%type <var> lvalue lvalue_ext
%type <explist> expseq expseq1 args args1
%type <type_p> ty
%type <flist> tyfields tyfields1
%type <fdeclist> funcdec
%type <dec_List> decs
%type <name_type> typedec
%type <decl> vardec
%type <e_list> recfield recfield1

%token
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK
  LBRACE RBRACE DOT
  PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
  AND OR ASSIGN
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
  BREAK NIL
  FUNCTION VAR TYPE

/* The following priority is unsure and need to modify when
 * needed 
 */
%nonassoc OF
%nonassoc IF THEN WHILE DO FOR TO
%left ELSE
%nonassoc ASSIGN
%left AND
%left OR
%nonassoc EQ NEQ GT LT GE LE
%left PLUS MINUS
%left TIMES DIVIDE

%start ProgramRoot

%%

ProgramRoot:
	exp { 
		root = $1;
	}
	;
exp:
	STRING { $$ = A_StringExp(EM_tokPos,yyval.sval); }
	|INTT { $$ = A_IntExp(EM_tokPos,yylval.ival); }
	|NIL {$$ = A_NilExp(EM_tokPos);}
	|lvalue	{ $$ = A_VarExp(EM_tokPos,$1); }
	|MINUS exp {$$ = A_OpExp(EM_tokPos,A_minusOp,A_IntExp(EM_tokPos,0),$2);}
		| exp TIMES exp { $$ = A_OpExp(EM_tokPos,A_timesOp,$1,$3);}
		| exp DIVIDE exp { $$ = A_OpExp(EM_tokPos,A_divideOp,$1,$3);}
		| exp PLUS exp { $$ = A_OpExp(EM_tokPos,A_plusOp,$1,$3); }
		| exp MINUS exp { $$ = A_OpExp(EM_tokPos, A_minusOp,$1,$3);}
		| exp EQ exp { $$ = A_OpExp(EM_tokPos,A_eqOp,$1,$3);}
		| exp NEQ exp { $$ = A_OpExp(EM_tokPos,A_neqOp,$1,$3);}
		| exp LT exp { $$ = A_OpExp(EM_tokPos,A_ltOp,$1,$3);}
		| exp LE exp { $$ = A_OpExp(EM_tokPos,A_leOp,$1,$3);}
		| exp GT exp { $$ = A_OpExp(EM_tokPos,A_gtOp,$1,$3);}
		| exp GE exp { $$ = A_OpExp(EM_tokPos,A_geOp,$1,$3);}
		| exp AND exp {$$ = A_IfExp(EM_tokPos,$1,$3,A_IntExp(EM_tokPos,0));}
		| exp OR exp { $$ = A_IfExp(EM_tokPos,$1,A_IntExp(EM_tokPos,1),$3);}
	|lvalue ASSIGN exp {$$ = A_AssignExp(EM_tokPos,$1,$3);}
	|id LPAREN args RPAREN { $$ = A_CallExp(EM_tokPos,$1,$3);} /*id (expr-list) function dec*/
	|LPAREN expseq RPAREN  { $$ = A_SeqExp(EM_tokPos,$2); } /* (expr-seq) */
	|id LBRACE recfield RBRACE {$$ = A_RecordExp(EM_tokPos,$1,$3);} /*type-id{field-list*/
	|id LBRACK exp RBRACK OF exp { $$ = A_ArrayExp(EM_tokPos,$1,$3,$6);}
	|IF exp THEN exp {$$ = A_IfExp(EM_tokPos,$2,$4,A_NilExp(EM_tokPos));}
	|IF exp THEN exp ELSE exp {$$ = A_IfExp(EM_tokPos,$2,$4,$6);}
	|WHILE exp DO exp { $$ = A_WhileExp(EM_tokPos,$2,$4);}
	|FOR id ASSIGN exp TO exp DO exp {$$ = A_ForExp(EM_tokPos,$2,$4,$6,$8);}
	|BREAK {$$=A_BreakExp(EM_tokPos);}
	|LET decs IN expseq END { $$ = A_LetExp(EM_tokPos,$2,A_SeqExp(EM_tokPos,$4));}
	;

id:	ID {$$ = S_Symbol($1);};

lvalue: id lvalue_ext { LVALUE_ACTION($$,$2,A_SimpleVar(EM_tokPos,$1));} /* Unknow reason for LAVALUE_ACTION */
	  ;

lvalue_ext:
	/* empty */ 	{ $$ = NULL; }
	|   DOT id lvalue_ext    { LVALUE_ACTION($$, $3, A_FieldVar(EM_tokPos, NULL, $2)); }
	|   LBRACK exp RBRACK lvalue_ext    { LVALUE_ACTION($$, $4, A_SubscriptVar(EM_tokPos, NULL, $2)); }
	;

args:   
	/* empty	 */ { $$ = NULL; }                   
	|  args1    { $$ = A_ExpList($1->head,$1->tail);}                  
	;

args1:  
	exp         { $$ = A_ExpList($1,NULL);}                   
	|  exp COMMA args1 	{ $$ = A_ExpList($1,$3);}
	;

expseq: 
	/*empty*/   { $$ = NULL; }                   
        | expseq1   { $$ = A_ExpList($1->head,$1->tail);}                 
		;

expseq1:
	exp        { $$ = A_ExpList($1,NULL);}                   
        |  exp SEMICOLON expseq1  { $$ = A_ExpList($1,$3);}
		;

decs:	/* empty */		{ $$ = NULL; }
	| typedec decs  	{ $$ = A_DecList(A_TypeDec(EM_tokPos, A_NametyList($1,NULL)),$2); }
	|   vardec decs		{ $$ =   A_DecList($1,$2); }
	|   funcdec decs	{ $$ =  A_DecList(A_FunctionDec(EM_tokPos, $1),$2); }
	;

vardec:    VAR id ASSIGN exp      { $$ = A_VarDec(EM_tokPos, $2, NULL, $4); }     
    |  VAR id COLON id ASSIGN exp  {$$ = A_VarDec(EM_tokPos, $2, $4, $6); }
	;

typedec:   TYPE id EQ ty	{ $$ = A_Namety($2, $4); }
	   ;

funcdec:   FUNCTION id LPAREN tyfields RPAREN EQ exp   { $$ = A_FundecList(A_Fundec(EM_tokPos, $2, $4, NULL, $7),NULL); }
 	|  FUNCTION id LPAREN tyfields RPAREN COLON id EQ exp 	{ $$ = A_FundecList(A_Fundec(EM_tokPos, $2, $4, $7, $9),NULL); }
	;

ty:     id          { $$ = A_NameTy(EM_tokPos, $1); }               
        |  LBRACE tyfields RBRACE     { $$ = A_RecordTy(EM_tokPos, $2); } 
        |  ARRAY OF id 	 { $$ = A_ArrayTy(EM_tokPos, $3); }
		;

tyfields: /* empty */   { $$ = NULL; }                
      | tyfields1   {$$ = A_FieldList($1->head,$1->tail);}
	  ;

tyfields1: id COLON id   { $$ = A_FieldList(A_Field(EM_tokPos,$1,$3),NULL);} 	             
        |  id COLON id COMMA tyfields1 { $$ = A_FieldList(A_Field(EM_tokPos,$1,$3),$5);}
		;

recfield: /* empty */ {$$ = NULL;}                
        |  recfield1  { $$ = A_EfieldList($1->head,$1->tail);}
		;

recfield1: id EQ exp  { $$ = A_EfieldList(A_Efield($1,$3),NULL);}         
        |  id EQ exp COMMA recfield1 { $$ = A_EfieldList(A_Efield($1,$3),$5);} 
		;

%%
yyerror(char *s){
	return 0;
}

