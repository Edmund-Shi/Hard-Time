
/*  A Bison parser, made from frame.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ID	257
#define	INTT	258
#define	STRING	259
#define	COMMA	260
#define	COLON	261
#define	SEMICOLON	262
#define	LPAREN	263
#define	RPAREN	264
#define	LBRACK	265
#define	RBRACK	266
#define	LBRACE	267
#define	RBRACE	268
#define	DOT	269
#define	PLUS	270
#define	MINUS	271
#define	TIMES	272
#define	DIVIDE	273
#define	EQ	274
#define	NEQ	275
#define	LT	276
#define	LE	277
#define	GT	278
#define	GE	279
#define	AND	280
#define	OR	281
#define	ASSIGN	282
#define	ARRAY	283
#define	IF	284
#define	THEN	285
#define	ELSE	286
#define	WHILE	287
#define	FOR	288
#define	TO	289
#define	DO	290
#define	LET	291
#define	IN	292
#define	END	293
#define	OF	294
#define	BREAK	295
#define	NIL	296
#define	FUNCTION	297
#define	VAR	298
#define	TYPE	299

#line 1 "frame.y"

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


#line 29 "frame.y"
typedef union {
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
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		140
#define	YYFLAG		-32768
#define	YYNTBASE	46

#define YYTRANSLATE(x) ((unsigned)(x) <= 299 ? yytranslate[x] : 64)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,    10,    13,    17,    21,    25,
    29,    33,    37,    41,    45,    49,    53,    57,    61,    65,
    70,    74,    79,    86,    91,    98,   103,   112,   114,   120,
   122,   125,   126,   130,   135,   136,   138,   140,   144,   145,
   147,   149,   153,   154,   157,   160,   163,   168,   175,   180,
   188,   198,   200,   204,   208,   209,   211,   215,   221,   222,
   224,   228
};

static const short yyrhs[] = {    47,
     0,     5,     0,     4,     0,    42,     0,    49,     0,    17,
    47,     0,    47,    18,    47,     0,    47,    19,    47,     0,
    47,    16,    47,     0,    47,    17,    47,     0,    47,    20,
    47,     0,    47,    21,    47,     0,    47,    22,    47,     0,
    47,    23,    47,     0,    47,    24,    47,     0,    47,    25,
    47,     0,    47,    26,    47,     0,    47,    27,    47,     0,
    49,    28,    47,     0,    48,     9,    51,    10,     0,     9,
    53,    10,     0,    48,    13,    62,    14,     0,    48,    11,
    47,    12,    40,    47,     0,    30,    47,    31,    47,     0,
    30,    47,    31,    47,    32,    47,     0,    33,    47,    36,
    47,     0,    34,    48,    28,    47,    35,    47,    36,    47,
     0,    41,     0,    37,    55,    38,    53,    39,     0,     3,
     0,    48,    50,     0,     0,    15,    48,    50,     0,    11,
    47,    12,    50,     0,     0,    52,     0,    47,     0,    47,
     6,    52,     0,     0,    54,     0,    47,     0,    47,     8,
    54,     0,     0,    57,    55,     0,    56,    55,     0,    58,
    55,     0,    44,    48,    28,    47,     0,    44,    48,     7,
    48,    28,    47,     0,    45,    48,    20,    59,     0,    43,
    48,     9,    60,    10,    20,    47,     0,    43,    48,     9,
    60,    10,     7,    48,    20,    47,     0,    48,     0,    13,
    60,    14,     0,    29,    40,    48,     0,     0,    61,     0,
    48,     7,    48,     0,    48,     7,    48,     6,    61,     0,
     0,    63,     0,    48,    20,    47,     0,    48,    20,    47,
     6,    63,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    87,    94,    96,    97,    98,    99,   100,   101,   102,   103,
   104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
   114,   115,   116,   117,   118,   119,   120,   121,   122,   125,
   127,   130,   132,   133,   136,   138,   141,   143,   146,   148,
   151,   153,   156,   157,   158,   159,   162,   163,   166,   169,
   170,   173,   174,   175,   178,   179,   182,   183,   186,   187,
   190,   191
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","ID","INTT",
"STRING","COMMA","COLON","SEMICOLON","LPAREN","RPAREN","LBRACK","RBRACK","LBRACE",
"RBRACE","DOT","PLUS","MINUS","TIMES","DIVIDE","EQ","NEQ","LT","LE","GT","GE",
"AND","OR","ASSIGN","ARRAY","IF","THEN","ELSE","WHILE","FOR","TO","DO","LET",
"IN","END","OF","BREAK","NIL","FUNCTION","VAR","TYPE","ProgramRoot","exp","id",
"lvalue","lvalue_ext","args","args1","expseq","expseq1","decs","vardec","typedec",
"funcdec","ty","tyfields","tyfields1","recfield","recfield1", NULL
};
#endif

static const short yyr1[] = {     0,
    46,    47,    47,    47,    47,    47,    47,    47,    47,    47,
    47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
    47,    47,    47,    47,    47,    47,    47,    47,    47,    48,
    49,    50,    50,    50,    51,    51,    52,    52,    53,    53,
    54,    54,    55,    55,    55,    55,    56,    56,    57,    58,
    58,    59,    59,    59,    60,    60,    61,    61,    62,    62,
    63,    63
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     1,     2,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     4,
     3,     4,     6,     4,     6,     4,     8,     1,     5,     1,
     2,     0,     3,     4,     0,     1,     1,     3,     0,     1,
     1,     3,     0,     2,     2,     2,     4,     6,     4,     7,
     9,     1,     3,     3,     0,     1,     3,     5,     0,     1,
     3,     5
};

static const short yydefact[] = {     0,
    30,     3,     2,    39,     0,     0,     0,     0,    43,    28,
     4,     1,    32,     5,    41,     0,    40,     6,     0,     0,
     0,     0,     0,     0,     0,    43,    43,    43,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    35,     0,    59,     0,    31,     0,     0,    21,     0,     0,
     0,     0,     0,     0,    39,    45,    44,    46,     9,    10,
     7,     8,    11,    12,    13,    14,    15,    16,    17,    18,
    37,     0,    36,     0,     0,     0,    60,    32,    19,    42,
    24,    26,     0,    55,     0,     0,     0,     0,     0,    20,
    32,     0,    22,     0,    33,     0,     0,     0,     0,    56,
     0,    47,    55,     0,    52,    49,    29,    38,     0,    34,
    61,     0,    25,     0,     0,     0,     0,     0,     0,    23,
     0,    32,     0,    57,     0,     0,    48,    53,    54,    62,
    27,     0,     0,    50,    58,     0,    51,     0,     0,     0
};

static const short yydefgoto[] = {   138,
    15,    13,    14,   110,    72,    73,    16,    17,    25,    26,
    27,    28,   106,    99,   100,    76,    77
};

static const short yypact[] = {    65,
-32768,-32768,-32768,    65,    65,    65,    65,    22,   -22,-32768,
-32768,   321,     5,   -20,   228,    16,-32768,    -8,   277,   186,
    17,    22,    22,    22,   -11,   -22,   -22,   -22,    65,    65,
    65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
    65,    65,    22,    22,-32768,    65,    65,-32768,    65,    65,
    65,    38,    -4,    28,    65,-32768,-32768,-32768,    -8,    -8,
-32768,-32768,   333,   333,   333,   333,   333,   333,   124,   343,
   152,    45,-32768,   293,    36,    46,-32768,    -2,   321,-32768,
   260,   321,   240,    22,    22,    65,    -1,    23,    65,-32768,
     4,    65,-32768,    65,-32768,    65,    65,    54,    53,-32768,
    37,   321,    22,    24,-32768,-32768,-32768,-32768,    65,-32768,
   174,   309,   321,   207,    22,    -3,    65,    57,    22,   321,
    22,    -2,    65,    61,    22,    65,   321,-32768,-32768,-32768,
   321,    22,    52,   321,-32768,    65,   321,    73,    75,-32768
};

static const short yypgoto[] = {-32768,
     0,    35,-32768,   -12,-32768,   -13,    25,    30,    26,-32768,
-32768,-32768,-32768,   -19,   -51,-32768,   -38
};


#define	YYLAST		368


static const short yytable[] = {    12,
    45,     1,    85,   125,    18,    19,    20,    46,    94,    31,
    32,   103,    44,    41,    94,    42,   126,    43,    44,    44,
    22,    23,    24,    86,     1,    48,    55,   104,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    71,    74,    21,   109,    51,    79,    84,    87,    81,    82,
    83,    56,    57,    58,    90,    92,    52,    53,    54,    93,
   115,   107,   116,   119,   117,    95,   132,     1,     2,     3,
   128,   136,   139,     4,   140,   108,    80,    75,    78,    88,
   135,     5,   130,   118,     0,   102,     0,     0,    71,     0,
     0,   111,     0,   112,     6,   113,   114,     7,     8,     0,
     0,     9,     0,     0,     0,    10,    11,     0,   120,     0,
     0,     0,     0,     0,     0,     0,   127,     0,    98,   101,
     0,   105,   131,     0,     0,   134,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   137,     0,    98,     0,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,   124,
    40,     0,     0,   129,     0,    75,     0,    89,     0,   133,
     0,     0,     0,     0,     0,     0,    98,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,   121,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,     0,     0,     0,     0,     0,     0,     0,
     0,    50,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,     0,    47,     0,     0,     0,     0,
     0,     0,   123,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,     0,     0,
     0,     0,     0,     0,    97,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,     0,     0,
     0,    96,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    91,     0,     0,    49,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
   122,     0,     0,     0,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    29,    30,
    31,    32,-32768,-32768,-32768,-32768,-32768,-32768,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38
};

static const short yycheck[] = {     0,
    13,     3,     7,     7,     5,     6,     7,    28,    11,    18,
    19,    13,    15,     9,    11,    11,    20,    13,    15,    15,
    43,    44,    45,    28,     3,    10,    38,    29,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,     8,    40,    28,    46,     9,    20,    49,    50,
    51,    26,    27,    28,    10,    20,    22,    23,    24,    14,
     7,    39,    10,    40,    28,    78,     6,     3,     4,     5,
    14,    20,     0,     9,     0,    89,    47,    43,    44,    55,
   132,    17,   121,   103,    -1,    86,    -1,    -1,    89,    -1,
    -1,    92,    -1,    94,    30,    96,    97,    33,    34,    -1,
    -1,    37,    -1,    -1,    -1,    41,    42,    -1,   109,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    84,    85,
    -1,    87,   123,    -1,    -1,   126,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   136,    -1,   103,    -1,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,   115,
    27,    -1,    -1,   119,    -1,   121,    -1,     6,    -1,   125,
    -1,    -1,    -1,    -1,    -1,    -1,   132,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    26,    27,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    36,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    -1,     8,    -1,    -1,    -1,    -1,
    -1,    -1,    36,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    35,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    -1,    -1,    -1,
    -1,    32,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    12,    -1,    -1,    31,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc)
#include <alloca.h>
#else /* not sparc */
#if (defined (MSDOS) || defined(WIN32)) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#define YYLEX		yylex(&yylval, &yylloc)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_bcopy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif


#line 169 "bison.simple"
int
yyparse()
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
#ifdef YYLSP_NEEDED
		 &yyls1, size * sizeof (*yylsp),
#endif
		 &yystacksize);

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 88 "frame.y"
{ 
		Log("Parser","Begin");
		root = yyvsp[0].exp;
		Log("Parser","End");
	;
    break;}
case 2:
#line 95 "frame.y"
{ yyval.exp = A_StringExp(EM_tokPos,yyval.sval); ;
    break;}
case 3:
#line 96 "frame.y"
{ yyval.exp = A_IntExp(EM_tokPos,yylval.ival); ;
    break;}
case 4:
#line 97 "frame.y"
{yyval.exp = A_NilExp(EM_tokPos);;
    break;}
case 5:
#line 98 "frame.y"
{ yyval.exp = A_VarExp(EM_tokPos,yyvsp[0].var); ;
    break;}
case 6:
#line 99 "frame.y"
{yyval.exp = A_OpExp(EM_tokPos,A_minusOp,A_IntExp(EM_tokPos,0),yyvsp[0].exp);;
    break;}
case 7:
#line 100 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_timesOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 8:
#line 101 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_divideOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 9:
#line 102 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_plusOp,yyvsp[-2].exp,yyvsp[0].exp); ;
    break;}
case 10:
#line 103 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos, A_minusOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 11:
#line 104 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_eqOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 12:
#line 105 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_neqOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 13:
#line 106 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_ltOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 14:
#line 107 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_leOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 15:
#line 108 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_gtOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 16:
#line 109 "frame.y"
{ yyval.exp = A_OpExp(EM_tokPos,A_geOp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 17:
#line 110 "frame.y"
{yyval.exp = A_IfExp(EM_tokPos,yyvsp[-2].exp,yyvsp[0].exp,A_IntExp(EM_tokPos,0));;
    break;}
case 18:
#line 111 "frame.y"
{ yyval.exp = A_IfExp(EM_tokPos,yyvsp[-2].exp,A_IntExp(EM_tokPos,1),yyvsp[0].exp);;
    break;}
case 19:
#line 112 "frame.y"
{yyval.exp = A_AssignExp(EM_tokPos,yyvsp[-2].var,yyvsp[0].exp);;
    break;}
case 20:
#line 113 "frame.y"
{ yyval.exp = A_CallExp(EM_tokPos,yyvsp[-3].sym,yyvsp[-1].explist);;
    break;}
case 21:
#line 114 "frame.y"
{ yyval.exp = A_SeqExp(EM_tokPos,yyvsp[-1].explist); ;
    break;}
case 22:
#line 115 "frame.y"
{yyval.exp = A_RecordExp(EM_tokPos,yyvsp[-3].sym,yyvsp[-1].e_list);;
    break;}
case 23:
#line 116 "frame.y"
{ yyval.exp = A_ArrayExp(EM_tokPos,yyvsp[-5].sym,yyvsp[-3].exp,yyvsp[0].exp);;
    break;}
case 24:
#line 117 "frame.y"
{yyval.exp = A_IfExp(EM_tokPos,yyvsp[-2].exp,yyvsp[0].exp,A_NilExp(EM_tokPos));;
    break;}
case 25:
#line 118 "frame.y"
{yyval.exp = A_IfExp(EM_tokPos,yyvsp[-4].exp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 26:
#line 119 "frame.y"
{ yyval.exp = A_WhileExp(EM_tokPos,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 27:
#line 120 "frame.y"
{yyval.exp = A_ForExp(EM_tokPos,yyvsp[-6].sym,yyvsp[-4].exp,yyvsp[-2].exp,yyvsp[0].exp);;
    break;}
case 28:
#line 121 "frame.y"
{yyval.exp=A_BreakExp(EM_tokPos);;
    break;}
case 29:
#line 122 "frame.y"
{ yyval.exp = A_LetExp(EM_tokPos,yyvsp[-3].dec_List,A_SeqExp(EM_tokPos,yyvsp[-1].explist));;
    break;}
case 30:
#line 125 "frame.y"
{yyval.sym = S_Symbol(yyvsp[0].sval);;
    break;}
case 31:
#line 127 "frame.y"
{ LVALUE_ACTION(yyval.var,yyvsp[0].var,A_SimpleVar(EM_tokPos,yyvsp[-1].sym));;
    break;}
case 32:
#line 131 "frame.y"
{ yyval.var = NULL; ;
    break;}
case 33:
#line 132 "frame.y"
{ LVALUE_ACTION(yyval.var, yyvsp[0].var, A_FieldVar(EM_tokPos, NULL, yyvsp[-1].sym)); ;
    break;}
case 34:
#line 133 "frame.y"
{ LVALUE_ACTION(yyval.var, yyvsp[0].var, A_SubscriptVar(EM_tokPos, NULL, yyvsp[-2].exp)); ;
    break;}
case 35:
#line 137 "frame.y"
{ yyval.explist = NULL; ;
    break;}
case 36:
#line 138 "frame.y"
{ yyval.explist = A_ExpList(yyvsp[0].explist->head,yyvsp[0].explist->tail);;
    break;}
case 37:
#line 142 "frame.y"
{ yyval.explist = A_ExpList(yyvsp[0].exp,NULL);;
    break;}
case 38:
#line 143 "frame.y"
{ yyval.explist = A_ExpList(yyvsp[-2].exp,yyvsp[0].explist);;
    break;}
case 39:
#line 147 "frame.y"
{ yyval.explist = NULL; ;
    break;}
case 40:
#line 148 "frame.y"
{ yyval.explist = A_ExpList(yyvsp[0].explist->head,yyvsp[0].explist->tail);;
    break;}
case 41:
#line 152 "frame.y"
{ yyval.explist = A_ExpList(yyvsp[0].exp,NULL);;
    break;}
case 42:
#line 153 "frame.y"
{ yyval.explist = A_ExpList(yyvsp[-2].exp,yyvsp[0].explist);;
    break;}
case 43:
#line 156 "frame.y"
{ yyval.dec_List = NULL; ;
    break;}
case 44:
#line 157 "frame.y"
{ yyval.dec_List = A_DecList(A_TypeDec(EM_tokPos, A_NametyList(yyvsp[-1].name_type,NULL)),yyvsp[0].dec_List); ;
    break;}
case 45:
#line 158 "frame.y"
{ yyval.dec_List =   A_DecList(yyvsp[-1].decl,yyvsp[0].dec_List); ;
    break;}
case 46:
#line 159 "frame.y"
{ yyval.dec_List =  A_DecList(A_FunctionDec(EM_tokPos, yyvsp[-1].fdeclist),yyvsp[0].dec_List); ;
    break;}
case 47:
#line 162 "frame.y"
{ yyval.decl = A_VarDec(EM_tokPos, yyvsp[-2].sym, NULL, yyvsp[0].exp); ;
    break;}
case 48:
#line 163 "frame.y"
{yyval.decl = A_VarDec(EM_tokPos, yyvsp[-4].sym, yyvsp[-2].sym, yyvsp[0].exp); ;
    break;}
case 49:
#line 166 "frame.y"
{ yyval.name_type = A_Namety(yyvsp[-2].sym, yyvsp[0].type_p); ;
    break;}
case 50:
#line 169 "frame.y"
{ yyval.fdeclist = A_FundecList(A_Fundec(EM_tokPos, yyvsp[-5].sym, yyvsp[-3].flist, NULL, yyvsp[0].exp),NULL); ;
    break;}
case 51:
#line 170 "frame.y"
{ yyval.fdeclist = A_FundecList(A_Fundec(EM_tokPos, yyvsp[-7].sym, yyvsp[-5].flist, yyvsp[-2].sym, yyvsp[0].exp),NULL); ;
    break;}
case 52:
#line 173 "frame.y"
{ yyval.type_p = A_NameTy(EM_tokPos, yyvsp[0].sym); ;
    break;}
case 53:
#line 174 "frame.y"
{ yyval.type_p = A_RecordTy(EM_tokPos, yyvsp[-1].flist); ;
    break;}
case 54:
#line 175 "frame.y"
{ yyval.type_p = A_ArrayTy(EM_tokPos, yyvsp[0].sym); ;
    break;}
case 55:
#line 178 "frame.y"
{ yyval.flist = NULL; ;
    break;}
case 56:
#line 179 "frame.y"
{yyval.flist = A_FieldList(yyvsp[0].flist->head,yyvsp[0].flist->tail);;
    break;}
case 57:
#line 182 "frame.y"
{ yyval.flist = A_FieldList(A_Field(EM_tokPos,yyvsp[-2].sym,yyvsp[0].sym),NULL);;
    break;}
case 58:
#line 183 "frame.y"
{ yyval.flist = A_FieldList(A_Field(EM_tokPos,yyvsp[-4].sym,yyvsp[-2].sym),yyvsp[0].flist);;
    break;}
case 59:
#line 186 "frame.y"
{yyval.e_list = NULL;;
    break;}
case 60:
#line 187 "frame.y"
{ yyval.e_list = A_EfieldList(yyvsp[0].e_list->head,yyvsp[0].e_list->tail);;
    break;}
case 61:
#line 190 "frame.y"
{ yyval.e_list = A_EfieldList(A_Efield(yyvsp[-2].sym,yyvsp[0].exp),NULL);;
    break;}
case 62:
#line 191 "frame.y"
{ yyval.e_list = A_EfieldList(A_Efield(yyvsp[-4].sym,yyvsp[-2].exp),yyvsp[0].e_list);;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 442 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  for (x = 0; x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = 0; x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 194 "frame.y"

yyerror(char *s){
	return 0;
}

