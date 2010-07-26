
# line 7 "Parse.y"

#include <stdio.h>
#include <ctype.h>

#include "O.h"

FILE	*fopen();
extern FILE *fpin, *fpout;

extern A_OBJECT	A, Atemp;
extern T_OBJECT	TAlist;
extern A_OBJECT	Alist[100];
extern T_OBJECT	TT;

extern int disp_flag;

extern int do_n_i();
extern A_OBJECT do_an_a();
extern A_OBJECT do_ann_a();
extern A_OBJECT do_nn_a();

int		i, num;
char *		t;

extern char * pad20();
	
#ifdef UNIX
#define PROMT	if(isatty(fileno(fpin))&&isatty(fileno(fpout)))printf("--* ");
#else
#define PROMT	
#endif


# line 41 "Parse.y"
typedef union  {
	char *		up;
	A_OBJECT	uA;
} YYSTYPE;
# define EXCLAM 257
# define DOLLAR 258
# define PERCENT 259
# define AMPERSAND 260
# define LPAREN 261
# define RPAREN 262
# define STAR 263
# define PLUS 264
# define COMMA 265
# define MINUS 266
# define SLASH 267
# define COLON 268
# define SEMI 269
# define EQUAL 270
# define QUESTION 271
# define AT 272
# define LBRACK 273
# define BSLASH 274
# define RBRACK 275
# define CIRCUMFLEX 276
# define LBRACE 277
# define VBAR 278
# define RBRACE 279
# define NAME 280
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 388 "Parse.y"

short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 51
# define YYLAST 171
short yyact[]={

  17,  41,  17,  53,  42,  49,  77,  39,  75,  39,
  74,  49,  16,  47,  16,  15,  18,  15,  18,  34,
  45,  34,   3,  68,  48,  17,  23,  17,  66,  61,
  48,  31,  39,  17,   6,  67,  19,  16,  17,  16,
  15,  18,  15,  18,  34,  16,   5,  17,  15,  18,
  16,  24,  34,  15,  18,  59,  73,  34,  55,  16,
  17,  66,  15,  18,  22,  17,  34,  51,  28,  83,
  72,  84,  16,  32,  52,  15,  18,  16,   8,  34,
  15,  18,  53,  13,  34,  53,  36,  35,   4,  27,
  66,  76,  21,  20,  37,  29,  69,  25,  26,  70,
  14,  30,  11,  54,   2,  38,  43,  46,  10,  12,
   1,  50,   9,  44,  33,  40,  65,   7,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  71,  62,  63,  78,   0,  58,  60,  56,  57,
   0,  64,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  81,
   0,   0,  82,   0,   0,   0,  33,   0,  79,   0,
  80 };
short yypact[]={

-1000,-234,-1000,-233,-176,-206,-254,-221,-161,-189,
-165,-243,-194,-196,-177,-1000,-236,-261,-259,-1000,
-1000,-256,-201,-195,-214,-196,-196,-223,-228,-196,
-196,-196,-196,-177,-1000,-1000,-1000,-1000,-240,-257,
-166,-1000,-236,-207,-209,-1000,-207,-1000,-270,-272,
-178,-274,-1000,-1000,-161,-196,-189,-189,-165,-196,
-165,-196,-243,-243,-194,-196,-250,-1000,-277,-1000,
-236,-207,-1000,-236,-1000,-193,-1000,-198,-161,-165,
-165,-207,-207,-1000,-1000 };
short yypgo[]={

   0,  88, 117,  78, 112, 108, 102, 109,  83, 100,
 115, 113, 110, 104 };
short yyr1[]={

   0,  12,  12,  12,  13,  13,  13,  13,  13,   1,
   1,   1,   1,   1,   2,   2,   2,   3,   3,   3,
   4,   4,   4,   4,   4,   5,   5,   5,   6,   6,
   7,   7,   8,   8,   9,   9,   9,   9,   9,   9,
   9,   9,   9,   9,  10,  10,  10,  10,  10,  11,
  11 };
short yyr2[]={

   0,   2,   3,   0,   2,   3,   4,   5,   3,   3,
   5,   4,   3,   1,   3,   4,   1,   3,   3,   1,
   3,   3,   4,   4,   1,   3,   3,   1,   3,   1,
   3,   1,   2,   1,   2,   2,   2,   1,   1,   3,
   3,   2,   3,   2,   3,   2,   2,   1,   1,   3,
   1 };
short yychk[]={

-1000, -12, -13, 256,  -1, 280, 268,  -2,  -3,  -4,
  -5,  -6,  -7,  -8,  -9, 276, 273, 261, 277, 269,
 269, 268, 270, 280, 272, 258, 259, 278, 257, 260,
 266, 274, 267,  -9, 280, 264, 263, 271,  -1, 268,
 -10, 262, 265,  -1, -11, 279,  -1, 269, 280, 261,
  -1, 268, 269, 280,  -3, 272,  -4,  -4,  -5, 278,
  -5, 257,  -6,  -6,  -7,  -8, 268, 275, 280, 262,
 265,  -1, 279, 265, 280, 280, 269, 280,  -3,  -5,
  -5,  -1,  -1, 262, 269 };
short yydef[]={

   3,  -2,   1,   0,   0,  38,   0,  13,  16,  19,
  24,  27,  29,  31,  33,  37,   0,   0,   0,   2,
   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  32,  38,  34,  35,  36,   0,   0,
   0,  41,  47,  48,   0,  43,  50,   5,   9,   0,
   0,   0,   8,  12,  14,   0,  17,  18,  20,   0,
  21,   0,  25,  26,  28,  30,   0,  39,   0,  40,
  46,  45,  42,   0,  11,   0,   6,   0,  15,  22,
  23,  44,  49,  10,   7 };
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif not lint

#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 2:
# line 59 "Parse.y"
{
		yyerrok;
		if ( A_report ) pr_time_diff();
		PROMT
	} break;
case 4:
# line 68 "Parse.y"
{
			A = yypvt[-1].uA;
			if ( disp_flag == 2 ) A = A_min( A );
			if ( disp_flag ) {
				(void) A_rept( A );
				fprintf( fpout, "\n" );
				(void) A_store( A, (char *) NULL, TT );
				fprintf( fpout, "\n" );
			}
			A_destroy( Alist[ 0 ] );
			Alist[ 0 ] = A;
			if ( A_report ) pr_time_diff();
			PROMT
		} break;
case 5:
# line 83 "Parse.y"
{
			A = A_enum( yypvt[-2].uA, TT, 1000 );
			A_destroy( Alist[ 0 ] );
			Alist[ 0 ] = A;
			if ( A_report ) pr_time_diff();
			PROMT
		} break;
case 6:
# line 91 "Parse.y"
{
			A = yypvt[-1].uA;
			if ( disp_flag == 2 ) A = A_min( A );
			if ( disp_flag ) {
				fprintf( fpout, "%s  ", pad20( yypvt[-3].up ) );
				(void) A_rept( A );
			}
			if ( (i = T_member( TAlist, yypvt[-3].up )) >= 0 )
				A_destroy( Alist[ i ] );
			Alist[ T_insert( TAlist, yypvt[-3].up ) ] = A;
			if ( T_member( TT, yypvt[-3].up ) >= 0 )
				fprintf( fpout,
					"Warning: %s is also a token\n", yypvt[-3].up );
			Sfree( yypvt[-3].up );
			if ( A_report ) pr_time_diff();
			PROMT
		} break;
case 7:
# line 109 "Parse.y"
{
			if ( !strcmp("read",yypvt[-1].up) || !strcmp("load",yypvt[-1].up) )
				A = A_load(yypvt[-4].up,TT);
			else {
				Warning( "Unknown function" );
				A = A_create();
			}
			fprintf( fpout, "%s  ", pad20( yypvt[-4].up ) );
			(void) A_rept( A );
			if ( (i = T_member( TAlist, yypvt[-4].up )) >= 0 )
				A_destroy( Alist[ i ] );
			Alist[ T_insert( TAlist, yypvt[-4].up ) ] = A;
			if ( T_member( TT, yypvt[-4].up ) >= 0 )
				fprintf( fpout,
					"Warning: %s is also a token\n", yypvt[-4].up );
			Sfree( yypvt[-4].up );
			Sfree( yypvt[-1].up );
			if ( A_report ) pr_time_diff();
			PROMT
		} break;
case 8:
# line 130 "Parse.y"
{
			i = do_n_i( yypvt[-1].up );
			Sfree( yypvt[-1].up );
			if ( i ) return;
			if ( A_report ) pr_time_diff();
			PROMT
		} break;
case 9:
# line 140 "Parse.y"
{
		yyval.uA = do_an_a( yypvt[-2].uA, yypvt[-0].up );
		Sfree( yypvt[-0].up );
	} break;
case 10:
# line 145 "Parse.y"
{
		disp_flag = 0;
		A = yypvt[-4].uA;
		if ( (num = tonum(yypvt[-1].up)) >= 0 ) {
			if ( A_report ) fprintf( fpout, "(@pow)\n" );
			A = A_cmpow( yypvt[-4].uA, num );
			disp_flag = 2;
		} else Warning( "Unknown function" );
		Sfree( yypvt[-1].up );
		yyval.uA = A;
	} break;
case 11:
# line 157 "Parse.y"
{
		A = do_ann_a( yypvt[-3].uA, yypvt[-1].up, yypvt[-0].up );
		Sfree( yypvt[-1].up );
		Sfree( yypvt[-0].up );
		yyval.uA = A;
	} break;
case 12:
# line 164 "Parse.y"
{
		A = do_nn_a( yypvt[-1].up, yypvt[-0].up );
		Sfree( yypvt[-1].up );
		Sfree( yypvt[-0].up );
		if ( A == NULL ) A = A_create();
		yyval.uA = A;
	} break;
case 13:
# line 172 "Parse.y"
{
		disp_flag = 2;
	} break;
case 14:
# line 178 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(@)\n" );
		yyval.uA = A_compose( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 15:
# line 183 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(@@)\n" );
		yyval.uA = A_join( yypvt[-3].uA, yypvt[-0].uA );
	} break;
case 17:
# line 191 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "($)\n" );
		yyval.uA = A_retape( yypvt[-2].uA, yypvt[-0].uA, TT );
	} break;
case 18:
# line 196 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(%)\n" );
		yyval.uA = A_percent( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 20:
# line 204 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(|)\n" );
		yyval.uA = A_union( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 21:
# line 209 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(!)\n" );
		yyval.uA = A_xor( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 22:
# line 214 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(||)\n" );
		if ( yypvt[-3].uA-> A_nT > 1 || yypvt[-0].uA-> A_nT > 1 ) {
			Atemp = A_differ(
					A_retape( A_copy( yypvt[-0].uA ),
					A_letter( 0, T_insert( TT, "0" ) ), TT
						),
					A_retape( A_copy( yypvt[-3].uA ),
					A_letter( 0, T_insert( TT, "0" ) ), TT
						)
			);
			yyval.uA = A_union( yypvt[-3].uA, A_join( Atemp, yypvt[-0].uA ) );
		} else	yyval.uA = A_union( yypvt[-3].uA, yypvt[-0].uA );
	} break;
case 23:
# line 229 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(!!)\n" );
		yyval.uA = A_shuffle( yypvt[-3].uA, yypvt[-0].uA );
	} break;
case 25:
# line 237 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(&)\n" );
		yyval.uA = A_intersect( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 26:
# line 242 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(-)\n" );
		yyval.uA = A_differ( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 28:
# line 250 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(\\)\n" );
		if ( yypvt[-2].uA-> A_nT > 1 || yypvt[-0].uA-> A_nT > 1 ) {
			fprintf( fpout,
				"Error: \\ applied to multitape automaton\n" );
			A_destroy( yypvt[-2].uA );
			A_destroy( yypvt[-0].uA );
			yyval.uA = A_phi();
		} else {
			Atemp = A_retape( A_star( A_alph( A_copy( yypvt[-0].uA ) ) ),
				A_comma(
					A_letter( 0, T_insert( TT, "0" ) ),
					A_letter( 0, T_insert( TT, "0" ) ) ),
				TT );
			yyval.uA = A_compose( yypvt[-0].uA, A_concat( yypvt[-2].uA, Atemp ) );
		}
	} break;
case 30:
# line 271 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(/)\n" );
		if ( yypvt[-2].uA-> A_nT > 1 || yypvt[-0].uA-> A_nT > 1 ) {
			fprintf( fpout,
				"Error: / applied to multitape automaton\n" );
			A_destroy( yypvt[-2].uA );
			A_destroy( yypvt[-0].uA );
			yyval.uA = A_phi();
		} else {
			Atemp = A_retape( A_star( A_alph( A_copy( yypvt[-2].uA ) ) ),
				A_comma(
					A_letter( 0, T_insert( TT, "0" ) ),
					A_letter( 0, T_insert( TT, "0" ) ) ),
				TT );
			yyval.uA = A_compose( yypvt[-2].uA, A_concat( Atemp, yypvt[-0].uA ) );
		}
	} break;
case 32:
# line 292 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "( )\n" );
		yyval.uA = A_concat( yypvt[-1].uA, yypvt[-0].uA );
	} break;
case 34:
# line 300 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(+)\n" );
		yyval.uA = A_plus( yypvt[-1].uA );
	} break;
case 35:
# line 305 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(*)\n" );
		yyval.uA = A_star( yypvt[-1].uA );
	} break;
case 36:
# line 310 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(?)\n" );
		yyval.uA = A_opt( yypvt[-1].uA );
	} break;
case 37:
# line 315 "Parse.y"
{
		yyval.uA = A_lambda();
	} break;
case 38:
# line 319 "Parse.y"
{
		t = yypvt[-0].up;
		if ( t[1] == '.' && t[0] >= '0' && t[0] <= '9' )
			yyval.uA = A_letter( t[0] - '0', T_insert( TT, t + 2 ) );
		else if ( (i = T_member( TAlist, t )) >= 0
			&& T_member( TT, t ) < 0 )
				yyval.uA = A_copy( Alist[ i ] );
		else {
			yyval.uA = A_letter( 0, T_insert( TT, t ) );
			if ( i >= 0 )
			fprintf( fpout,
				"Warning: %s is a variable and a token\n", t );
		}
		Sfree( yypvt[-0].up );
	} break;
case 39:
# line 335 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "([])\n" );
		yyval.uA = A_comma( A_lambda(), yypvt[-1].uA );
	} break;
case 40:
# line 340 "Parse.y"
{
		yyval.uA = yypvt[-1].uA;
	} break;
case 41:
# line 344 "Parse.y"
{
		yyval.uA = A_lambda();
	} break;
case 42:
# line 348 "Parse.y"
{
		yyval.uA = yypvt[-1].uA;
	} break;
case 43:
# line 352 "Parse.y"
{
		yyval.uA = A_phi();
	} break;
case 44:
# line 358 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		yyval.uA = A_comma( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 45:
# line 363 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		yyval.uA = A_comma( A_lambda(), yypvt[-0].uA );
	} break;
case 46:
# line 368 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		yyval.uA = A_comma( yypvt[-1].uA, A_lambda() );
	} break;
case 47:
# line 373 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		yyval.uA = A_comma( A_lambda(), A_lambda() );
	} break;
case 49:
# line 381 "Parse.y"
{
		if ( A_report ) fprintf( fpout, "(,|)\n" );
		yyval.uA = A_union( yypvt[-2].uA, yypvt[-0].uA );
	} break;
		}
		goto yystack;  /* stack new state and value */

	}
