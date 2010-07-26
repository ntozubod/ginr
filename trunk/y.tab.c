
# line 7 "inrat.y"

#include <stdio.h>
#include <ctype.h>

#include "O.h"

FILE	*fopen();
extern FILE *fpin, *fpout;

A_OBJECT	A, Atemp;
T_OBJECT	TAlist;
A_OBJECT	Alist[100];
T_OBJECT	TT;

int		i, flag, num;
char *		t;

char * pad20( s )
char * s;
{
	static char tmp[ 41 ];
	if ( strlen( s ) >= 20 ) return( s );
	else {
		(void) strcpy( tmp, s );
		(void) strcat( tmp, " -------------------" );
		tmp[ 20 ] = 0;
		return( tmp );
	}
}
	
#ifdef UNIX
#define PROMT	if(isatty(fileno(fpin))&&isatty(fileno(fpout)))printf("--* ");
#else
#define PROMT	
#endif


# line 45 "inrat.y"
typedef union  {
	char *		up;
	A_OBJECT	uA;
} YYSTYPE;
# define EXCLAM 257
# define DOLLAR 258
# define AMPERSAND 259
# define LPAREN 260
# define RPAREN 261
# define STAR 262
# define PLUS 263
# define COMMA 264
# define MINUS 265
# define SLASH 266
# define COLON 267
# define SEMI 268
# define EQUAL 269
# define QUESTION 270
# define AT 271
# define LBRACK 272
# define BSLASH 273
# define RBRACK 274
# define CIRCUMFLEX 275
# define LBRACE 276
# define VBAR 277
# define RBRACE 278
# define NAME 279
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 720 "inrat.y"


int	ch = ' ';
char	token[512];
int	in_string = 0;

char *copyof( str )
char *str;
{
	return( strcpy( Salloc( strlen( str ) + 1 ), str ) );
}

yylex()
{
	int li, d, lflag, in_comment;
	fflush( fpout );
	if ( in_string ) {
		ch = getc( fpin );
		if ( ch == '\'' ) {
			ch = getc( fpin );
			if ( ch != '\'' ) {
				in_string = 0;
				return( RPAREN );
			}
		}
		if ( ch == '\\' ) {
			ch = getc( fpin );
			switch( ch ) {
				case 'n': ch = '\n'; break;
				case 't': ch = '\t'; break;
				case '_': ch = ' ';  break;
			}
		}
		if ( ch == EOF ) Error( "End of file in string" );
		token[ 0 ] = ch;
		token[ 1 ] = 0;
		yylval.up = copyof( token );
		return( NAME );
	}
	in_comment = 0;
	while( ch == ' ' || ch == '\t' || ch == '\n' || ch == '#'
			 || in_comment ) {
		if ( ch == '#'  ) in_comment = 1;
		if ( ch == '\n' ) in_comment = 0;
		if ( ch == EOF  ) Error( "End of file in comment" );
		ch = getc( fpin );
	}
	if ( ch == EOF ) return( 0 );
	d = ch;
	ch = ' ';
	switch( d ) {
	case '!':	return( EXCLAM );
/*	case '"':	not used	*/
/*	case '#':	COMMENT		*/
	case '$':	return( DOLLAR );
/*	case '%':	not used	*/
	case '&':	return( AMPERSAND );
	case '\'':	in_string = 1; return( LPAREN );

	case '(':	return( LPAREN );
	case ')':	return( RPAREN );
	case '*':	return( STAR );
	case '+':	return( PLUS );
	case ',':	return( COMMA );
	case '-':	return( MINUS );
/*	case '.':	ALPHANUMERIC	*/
	case '/':	return( SLASH );

	case ':':	return( COLON );
	case ';':	return( SEMI );
/*	case '<':	not used	*/
	case '=':	return( EQUAL );
/*	case '>':	not used	*/
	case '?':	return( QUESTION );

	case '@':	return( AT );

	case '[':	return( LBRACK );
	case '\\':	return( BSLASH );
	case ']':	return( RBRACK );
	case '^':	return( CIRCUMFLEX );
/*	case '_':	ALPHANUMERIC	*/

/*	case '`':	TOKEN QUOTE	*/

	case '{':	return( LBRACE );
	case '|':	return( VBAR );
	case '}':	return( RBRACE );
/*	case '~':	not used	*/

#ifdef CMS
	case 0300:	return( LBRACE );
	case 0320:	return( RBRACE );
#endif
	case '"': case '%': case '<': case '>': case '~':
		fprintf( fpout, "Reserved character: %c\n", d );
		return( d );
	}
	li = 0;
	ch = d;
	lflag = 1;
	if ( ch == '`' ) {
		ch = getc( fpin );
		while ( ch != EOF ) {
			if ( ch == '`' ) {
				ch = getc( fpin );
				if ( ch != '`' ) break;
			}
			if ( ch == '\\' ) {
				ch = getc( fpin );
				switch( ch ) {
					case 'n': ch = '\n'; break;
					case 't': ch = '\t'; break;
					case '_': ch = ' ';  break;
				}
			}
			token[ li++ ] = ch;
			ch = getc( fpin );
		}
		if ( li == 0 ) return( CIRCUMFLEX );
	} else {
		while ( lflag && ch != EOF ) {
			token[ li++ ] = ch;
			ch = getc( fpin );

	if ( li!=2 || token[1]!='.' || !isdigit(token[0]) ) {
		lflag = 0;
		switch( ch ) {
			case '.': case '_':

			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':

			case 'A': case 'B': case 'C': case 'D': case 'E':
			case 'F': case 'G': case 'H': case 'I': case 'J':
			case 'K': case 'L': case 'M': case 'N': case 'O':
			case 'P': case 'Q': case 'R': case 'S': case 'T':
			case 'U': case 'V': case 'W': case 'X': case 'Y':
			case 'Z':

			case 'a': case 'b': case 'c': case 'd': case 'e':
			case 'f': case 'g': case 'h': case 'i': case 'j':
			case 'k': case 'l': case 'm': case 'n': case 'o':
			case 'p': case 'q': case 'r': case 's': case 't':
			case 'u': case 'v': case 'w': case 'x': case 'y':
			case 'z':
					lflag = 1; break;
		}
	}

		}
	}
	token[ li ] = 0;
	yylval.up = copyof( token );
	return( NAME );
}

char Notice[] = "Copyright (c) 1985, J Howard Johnson, University of Waterloo";
extern char Version[];
extern char Date[];

main( argc, argv )
int   argc;
char *argv[];
{
	int ti;
	char tstr[2];
	char file_in[50], file_out[50], rpt_out[50];

	fpin  = stdin;
	fpout = stdout;

	if( argc > 3) {
		printf( "Usage: inr [ input_file ] [ output_file ]\n" );
		exit (1) ;
	} 

	if( argc > 2 ) {
		strcpy( file_out, argv[2] ) ;
		fpout = fopen( file_out, "w" );
	}

	if( argc > 1 ) {
		strcpy( file_in, argv[1] ) ;
		fpin = fopen( file_in, "r" );
	}

	if (fpin == 0 || fpout == 0 ) {
		if( fpin )	 strcpy( rpt_out, file_out );
		else if (fpout ) strcpy( rpt_out, file_in );
		else {
			strcpy( rpt_out, file_in );
			strcat( rpt_out, ", " );
			strcat( rpt_out, file_out );
		}
	printf ( "Problem with %s file(s) opens. -- aborting\n", rpt_out) ;
		exit (1) ; 
	}
 
#ifdef CMS
	if ( isatty(fpout) ) {
#else
	if ( isatty(fileno(fpout)) ) {
#endif

fprintf( fpout, "\n" );
fprintf( fpout, "II  N     N  RRRRRR\n" );
fprintf( fpout, "II  N N   N  R    RR    I N R   Version %s\n", Version );
fprintf( fpout, "II  N  N  N  RRRRRR\n" );
fprintf( fpout, "II  N   N N  R    R         %s\n", Date );
fprintf( fpout, "II  N    NN  R     R" );
fprintf( fpout, "                                 (For help type   :help;)\n" );
fprintf( fpout, "\n" );
	} else { 
		fprintf( fpout, "I N R -- V %s, %s", Version, Date );
		if ( fpin != stdin )
			fprintf( fpout, "  (Source file: %s)", file_in ) ;
		fprintf( fpout, "\n\n\n" );
	}
	TT = T_create();
	if ( T_insert( TT, "^^" ) != 0
	  || T_insert( TT, "-|" ) != 1 ) Error( "main: Initializing TT" );
	tstr[1] = 0;
	for( ti = 1; ti <= 255; ti++ )
		if ( isascii(ti) && isprint(ti) || ti == '\t' || ti == '\n' ) {
			tstr[0] = ti;
			(void) T_insert( TT, tstr );
		}
	TAlist = T_create();
	if ( T_insert( TAlist, "_Last_" ) != 0 )
		Error( "main: Initializing TAlist" );
	Alist[ 0 ] = A_create();
	pr_time_diff();
	PROMT
	(void) yyparse();
	T_destroy( TT );
	T_destroy( TAlist );
	if ( A_report ) {
		fprintf( fpout, "\n" );
		T_stats();
		V_stats();
		R_stats();
		U_stats();
	}
	exit( 0 );
}

yyerror(str)
char *str;
{
	fprintf( fpout, "*** %s ***\n", str );
}

int tonum( p )
char *p;
{
	int acum, c;
	acum = 0;
	while ( c = *p++ ) {
		if ( c < '0' || c > '9' ) return( -1 );
		acum = acum * 10 + c - '0';
	}
	return( acum );
}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 51
# define YYLAST 169
short yyact[]={

  17,  40,  57,  52,  41,  17,  48,  38,  76,  74,
  73,  17,  16,  67,  46,  15,  18,  16,  38,  33,
  15,  18,  58,  16,  33,  47,  15,  18,   3,  44,
  33,  84,  17,  23,  17,  30,  24,  60,  48,   6,
  17,  38,  52,  27,  16,  22,  16,  15,  18,  15,
  18,   5,  16,  33,  17,  15,  18,  47,  19,  33,
  17,  31,  17,  26,  65,  54,  16,  50,  83,  15,
  18,  51,  16,  33,  16,  15,  18,  15,  18,  33,
  72,  33,  52,  25,  65,  35,  34,  65,  75,  21,
  20,  66,  28,  36,  71,   4,  68,  14,  29,  69,
  10,   8,  13,  11,  12,   9,   2,   1,  43,  39,
   7,  32,  37,  42,  45,   0,   0,   0,  49,   0,
   0,   0,   0,   0,   0,   0,  53,  56,  59,   0,
   0,  55,  61,  62,  64,  63,   0,  70,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  77,   0,  78,  79,
   0,  80,  32,   0,   0,  81,   0,   0,  82 };
short yypact[]={

-1000,-228,-1000,-210,-178,-224,-246,-235,-175,-214,
-167,-238,-205,-198,-177,-1000,-226,-260,-249,-1000,
-1000,-254,-200,-197,-206,-198,-255,-220,-198,-198,
-198,-198,-177,-1000,-1000,-1000,-1000,-183,-266,-165,
-1000,-226,-203,-184,-1000,-203,-1000,-269,-270,-180,
-271,-1000,-1000,-175,-198,-214,-167,-198,-198,-167,
-198,-238,-238,-205,-198,-222,-1000,-276,-1000,-226,
-203,-1000,-226,-1000,-193,-1000,-237,-175,-167,-167,
-167,-203,-203,-1000,-1000 };
short yypgo[]={

   0,  95, 110, 101, 105, 100, 103, 104, 102,  97,
 109, 108, 107, 106 };
short yyr1[]={

   0,  12,  12,  12,  13,  13,  13,  13,  13,   1,
   1,   1,   1,   1,   2,   2,   2,   3,   3,   4,
   4,   4,   4,   4,   4,   5,   5,   5,   6,   6,
   7,   7,   8,   8,   9,   9,   9,   9,   9,   9,
   9,   9,   9,   9,  10,  10,  10,  10,  10,  11,
  11 };
short yyr2[]={

   0,   2,   3,   0,   2,   3,   4,   5,   3,   3,
   5,   4,   3,   1,   3,   4,   1,   3,   1,   3,
   3,   4,   4,   4,   1,   3,   3,   1,   3,   1,
   3,   1,   2,   1,   2,   2,   2,   1,   1,   3,
   3,   2,   3,   2,   3,   2,   2,   1,   1,   3,
   1 };
short yychk[]={

-1000, -12, -13, 256,  -1, 279, 267,  -2,  -3,  -4,
  -5,  -6,  -7,  -8,  -9, 275, 272, 260, 276, 268,
 268, 267, 269, 279, 271, 258, 277, 257, 259, 265,
 273, 266,  -9, 279, 263, 262, 270,  -1, 267, -10,
 261, 264,  -1, -11, 278,  -1, 268, 279, 260,  -1,
 267, 268, 279,  -3, 271,  -4,  -5, 257, 277,  -5,
 257,  -6,  -6,  -7,  -8, 267, 274, 279, 261, 264,
  -1, 278, 264, 279, 279, 268, 279,  -3,  -5,  -5,
  -5,  -1,  -1, 261, 268 };
short yydef[]={

   3,  -2,   1,   0,   0,  38,   0,  13,  16,  18,
  24,  27,  29,  31,  33,  37,   0,   0,   0,   2,
   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  32,  38,  34,  35,  36,   0,   0,   0,
  41,  47,  48,   0,  43,  50,   5,   9,   0,   0,
   0,   8,  12,  14,   0,  17,  19,   0,   0,  20,
   0,  25,  26,  28,  30,   0,  39,   0,  40,  46,
  45,  42,   0,  11,   0,   6,   0,  15,  21,  22,
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
# line 63 "inrat.y"
{
		yyerrok;
		if ( A_report ) pr_time_diff();
		PROMT
	} break;
case 4:
# line 72 "inrat.y"
{
			A = yypvt[-1].uA;
			if ( flag == 2 ) A = A_min( A );
			if ( flag ) {
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
# line 87 "inrat.y"
{
			A = A_enum( yypvt[-2].uA, TT, 1000 );
			A_destroy( Alist[ 0 ] );
			Alist[ 0 ] = A;
			if ( A_report ) pr_time_diff();
			PROMT
		} break;
case 6:
# line 95 "inrat.y"
{
			A = yypvt[-1].uA;
			if ( flag == 2 ) A = A_min( A );
			if ( flag ) {
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
# line 113 "inrat.y"
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
# line 134 "inrat.y"
{
			if ( !strcmp("alph",yypvt[-1].up) ) {
				fprintf( fpout, "\n" );
				for( i = 2; i < TT-> T_n; i++ ) {
					fprintf( fpout, T_name( TT, i ) );
					fprintf( fpout, " " );
				}
				fprintf( fpout, "\n" );
			} else if ( !strcmp("free",yypvt[-1].up) ) {
				fprintf( fpout, "\n" );
				(void) Sarena();
				fprintf( fpout, "\n" );
			} else if ( !strcmp("list",yypvt[-1].up) ) {
				fprintf( fpout, "\n" );
				for( i = 0; i < TAlist-> T_n; i++ ) {
					fprintf( fpout, "%s  ", pad20(
						T_name( TAlist, i ) ) );
					(void) A_rept( Alist[ i ] );
				}
				fprintf( fpout, "\n" );
			} else if ( !strcmp("noreport",yypvt[-1].up) ) A_report = 0;
			else if ( !strcmp("pr",yypvt[-1].up) ) {
				for( i = 1; i < TAlist-> T_n; i++ )
					if ( Alist[ i ]-> A_nrows > 0 )
					(void) A_store( Alist[ i ],
						T_name( TAlist, i ), TT );
			} else if ( !strcmp("report",yypvt[-1].up) ) A_report = 1;
			else if ( !strcmp("save",yypvt[-1].up) ) {
				for( i = 1; i < TAlist-> T_n; i++ )
					if ( Alist[ i ]-> A_nrows > 0 )
					Alist[ i ] = A_save( Alist[ i ],
						T_name( TAlist, i ), TT );
			} else if ( !strcmp("time",yypvt[-1].up) ) pr_time_diff();
			else if( !strcmp("help",yypvt[-1].up) )

fprintf( fpout, "\n\
Basic Help\n\
\n\
To terminate session type       :bye;\n\
\n\
To get additional help type\n\
\n\
     :help ops;          (Priority and meaning of basic operators)\n\
     :help colonops;     (Colon operators)\n\
     :help ioops;        (IO operators)\n\
     :help stats;        (Form of statements)\n\n" );

			else if (  !strcmp("done",yypvt[-1].up)
				|| !strcmp("quit",yypvt[-1].up)
				|| !strcmp("stop",yypvt[-1].up)
				|| !strcmp("bye",yypvt[-1].up) ) return;
			else Warning( "Unknown function" );
			Sfree( yypvt[-1].up );
			if ( A_report ) pr_time_diff();
			PROMT
		} break;
case 9:
# line 193 "inrat.y"
{
		flag = 0;
		A = yypvt[-2].uA;
		if ( !strcmp("pr",yypvt[-0].up) ) {
			(void) A_rept( yypvt[-2].uA );
			fprintf( fpout, "\n" );
			A = A_store( yypvt[-2].uA, (char *) NULL, TT );
			fprintf( fpout, "\n" );
		} else if ( !strcmp("deecho",yypvt[-0].up) ) {
			A = yypvt[-2].uA;
			A = A_deecho( A,
				T_insert(TT,"ECHO") * A-> A_nT + 1,
				T_insert(TT,"NOECHO") * A-> A_nT + 1 );
			flag = 2;
		} else if ( !strcmp("prsseq",yypvt[-0].up) ) {
			fprintf( fpout, "\n" );
			A = A_prsseq( yypvt[-2].uA, (char *) NULL, TT );
			fprintf( fpout, "\n" );
		} else if ( !strcmp("acomp",yypvt[-0].up) ) {
			if ( A_report ) fprintf( fpout, "(acomp)\n" );
			Atemp = A_star( A_alph( A_copy( yypvt[-2].uA ) ) );
			A = A_differ( Atemp, yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("alph",yypvt[-0].up) ) {
			if ( A_report ) fprintf( fpout, "(alph)\n" );
			A = A_alph( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("card",yypvt[-0].up) ) {
			i = A_card( yypvt[-2].uA );
			if ( i < 0 ) fprintf( fpout, "Infinite\n" );
			else {
				fprintf( fpout, "%d word", i );
				if ( i != 1 ) fprintf( fpout, "s" );
				fprintf( fpout, "\n" );
			}
		} else if ( !strcmp("closed",yypvt[-0].up) ) {
			A = A_clsure( yypvt[-2].uA );
			flag = 1;
		} else if ( !strcmp("clsseq",yypvt[-0].up) ) {
			A = A_clsseq( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("comp",yypvt[-0].up) ) {
			if ( A_report ) fprintf( fpout, "(comp)\n" );
			if ( (i = T_member( TAlist, "SIGMA" )) >= 0 ) {
				Atemp = A_star( A_alph( A_copy( Alist[i] ) ) );
				A = A_differ( Atemp, yypvt[-2].uA );
			} else  fprintf( fpout,
					"Error in comp: SIGMA not defined\n" );
			flag = 2;
		} else if ( !strcmp("dfa",yypvt[-0].up) ) {
			A = A_subs( yypvt[-2].uA );
			flag = 1;
		} else if ( !strcmp("dfamin",yypvt[-0].up) ) {
			A = A_min( yypvt[-2].uA );
			flag = 1;
		} else if ( !strcmp("enum",yypvt[-0].up) ) {
			A = A_enum( yypvt[-2].uA, TT, 1000 );
		} else if ( !strcmp("limit",yypvt[-0].up) ) {
			if ( A_report ) fprintf( fpout, "(limit)\n" );
			if ( yypvt[-2].uA-> A_nT != 2 )
				Error( "Wrong number of tapes (limit)" );

		Atemp = A_differ(
				A_retape(
					A_copy( yypvt[-2].uA ),
					A_letter( 0, T_insert( TT, "1" ) ),
					TT
				),
				A_retape(
					A_copy( yypvt[-2].uA ),
					A_letter( 0, T_insert( TT, "0" ) ),
					TT
				)
		);

			A = A_join( yypvt[-2].uA, Atemp );
			flag = 2;
		} else if ( !strcmp("lamcm",yypvt[-0].up) ) {
			A = A_lamcm( yypvt[-2].uA );
			flag = 1;
		} else if ( !strcmp("lameq",yypvt[-0].up) ) {
			A = A_lameq( yypvt[-2].uA );
			flag = 1;
		} else if ( !strcmp("lenmin",yypvt[-0].up) ) {
			A = A_lenmin( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("min",yypvt[-0].up) ) {
			A = A_min( yypvt[-2].uA );
			flag = 1;
		} else if ( !strcmp("length",yypvt[-0].up) ) {
			i = A_minlen( yypvt[-2].uA );
			A = yypvt[-2].uA;
			if ( i < 0 ) fprintf( fpout, "Empty Set\n" );
			else {
				fprintf( fpout, "%d letter", i );
				if ( i != 1 ) fprintf( fpout, "s" );
				fprintf( fpout, "\n" );
			}
		} else if ( !strcmp("nfa",yypvt[-0].up) ) {
			A = A_close( yypvt[-2].uA );
			flag = 1;
		} else if ( !strcmp("pref",yypvt[-0].up) ) {
			if ( A_report ) fprintf( fpout, "(pref)\n" );
			A = A_pref( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("report",yypvt[-0].up) ) {
			A = A_rept( yypvt[-2].uA );
		} else if ( !strcmp("rev",yypvt[-0].up) ) {
			if ( A_report ) fprintf( fpout, "(rev)\n" );
			A = A_rev( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("sseq",yypvt[-0].up) ) {
			A = A_sseq( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("LMsseq",yypvt[-0].up) ) {
			A = A_LMsseq( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("GMsseq",yypvt[-0].up) ) {
			A = A_GMsseq( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("stems",yypvt[-0].up) ) {
			A_prstems( yypvt[-2].uA, TT, 0 );
		} else if ( !strcmp("suff",yypvt[-0].up) ) {
			if ( A_report ) fprintf( fpout, "(suff)\n" );
			A = A_suff( yypvt[-2].uA );
			flag = 2;
		} else if ( !strcmp("time",yypvt[-0].up) ) {
			pr_time_diff();
			flag = 2;
		} else if ( !strcmp("trim",yypvt[-0].up) ) {
			A = A_trim( yypvt[-2].uA );
			flag = 1;
		} else if ( !strcmp("update",yypvt[-0].up) ) {
			A = A_open( yypvt[-2].uA );
			flag = 1;
		} else if ( (num = tonum(yypvt[-0].up)) >= 0 ) {
			if ( A_report ) fprintf( fpout, "(pow)\n" );
			A = A_catpow( yypvt[-2].uA, num );
			flag = 2;
		} else  Warning( "Unknown function" );
		Sfree( yypvt[-0].up );
		yyval.uA = A;
	} break;
case 10:
# line 337 "inrat.y"
{
		flag = 0;
		A = yypvt[-4].uA;
		if ( (num = tonum(yypvt[-1].up)) >= 0 ) {
			if ( A_report ) fprintf( fpout, "(@pow)\n" );
			A = A_cmpow( yypvt[-4].uA, num );
			flag = 2;
		} else Warning( "Unknown function" );
		Sfree( yypvt[-1].up );
		yyval.uA = A;
	} break;
case 11:
# line 349 "inrat.y"
{
		flag = 0;
		A = yypvt[-3].uA;
		if ( !strcmp("enum",yypvt[-1].up) && (num = tonum(yypvt[-0].up)) >= 0)
			A = A_enum( yypvt[-3].uA, TT, num );
		else if ( !strcmp("save",yypvt[-1].up) )
			A = A_save( yypvt[-3].uA, yypvt[-0].up, TT );
		else if ( !strcmp("pr",yypvt[-1].up) ) 
			A = A_store( yypvt[-3].uA, yypvt[-0].up, TT );
		else if ( !strcmp("prsseq",yypvt[-1].up) ) 
			A = A_prsseq( yypvt[-3].uA, yypvt[-0].up, TT );
		else if ( !strcmp("stems",yypvt[-1].up) )
			A_prstems( yypvt[-3].uA, TT, tonum(yypvt[-0].up) );
		else if ( !strcmp("surgery",yypvt[-1].up) ) {
			num = tonum(yypvt[-0].up);
			if ( num < 2 || num >= yypvt[-3].uA-> A_nrows )
				Warning( "Illegal surgery" );
			else {
				A = A_open( yypvt[-3].uA );
				A = A_add( A, num,
					T_insert(TT,"INCISION") * A-> A_nT,
					num );
			}
		} else Warning( "Unknown function" );
		Sfree( yypvt[-1].up );
		Sfree( yypvt[-0].up );
		yyval.uA = A;
	} break;
case 12:
# line 378 "inrat.y"
{
		flag = 0;
		A = NULL;
		if ( !strcmp("get",yypvt[-1].up) ) {
			if ( (i = T_member( TAlist, yypvt[-0].up )) >= 0 )
				A = A_copy( Alist[i] );
			flag = 1;
		} else if ( !strcmp("read",yypvt[-1].up) || !strcmp("load",yypvt[-1].up) ) {
			A = A_load( yypvt[-0].up, TT );
			flag = 1;
		} else if ( !strcmp("readwords",yypvt[-1].up) ) {
			A = A_lwds( yypvt[-0].up, TT );
			flag = 1;
		} else if ( !strcmp("save",yypvt[-1].up) ) {
			if ( (i = T_member( TAlist, yypvt[-0].up )) >= 0 )
				A = A_save( A_copy( Alist[i] ), yypvt[-0].up, TT );
			else fprintf( fpout, "Warning: %s undefined\n", yypvt[-0].up );
		} else if ( !strcmp("pr",yypvt[-1].up) ) {
			if ( (i = T_member( TAlist, yypvt[-0].up )) >= 0 )
				A = A_store( Alist[i], yypvt[-0].up, TT );
			else fprintf( fpout, "Warning: %s undefined\n", yypvt[-0].up );
		} else if ( !strcmp("help",yypvt[-1].up) ) {
			if ( !strcmp("ops",yypvt[-0].up) )

fprintf( fpout, "\n\
Operations by priority (highest to lowest):\n\
\n\
+   *   ?               postfix operators for 1 or more, 0 or more, 0 or 1\n\
<concatenation>         no explicit operator\n\
\\   /                   left factor, right factor\n\
&   -                   intersection, set difference\n\
|   !   ||  !!          union, exclusive or, elseor, shuffle\n\
$                       project\n\
@   @@                  composition, join\n\
<all colon operators>   see :help colonops; for details\n\
,                       Cartesian product within (), union within {}\n\
\n\
All operators associate from left to right.\n\
Parentheses may be used to indicate a specific order of evaluation.\n\
{,,,} is a set constructor.\n\
(,,,) is a tuple construtor.\n\
[   ] is the tape-shifting operator\n\
'   ' is a string of single letter tokens.\n\
`   ` is a token containing arbitrary symbols.\n\
^     indicates the empty word (or an explicit concatenation operator).\n\n" );

			else if ( !strcmp("colonops",yypvt[-0].up) )

fprintf( fpout, "\n\
Colon operations (postfix operators at lowest priority)\n\
\n\
Transformation Operators               Displaying Operators\n\
:acomp      Active complement          :card       Print cardinality\n\
:alph       Active alphabet            :enum       Enumerate language\n\
:clsseq     Subsequential closure      :length     Display min word length\n\
:comp       Complement w.r.t. SIGMA*   :pr         Display automaton\n\
:lenmin     Words of min length        :prsseq     Subsequential display\n\
:pref       Set of prefixes            :report     Display report line\n\
:rev        Reverse                    :stems #    Print tape # stems\n\
:sseq       Subsequential transducer\n\
:LMsseq     LM Subsequential transducer\n\
:GMsseq     GM Subsequential transducer\n\
:suff       Set of suffixes            Coercing operators\n\
:<number>   Concatenation power        :update :nfa :trim :lameq\n\
:(<number>) Composition power          :lamcm :closed :dfa :dfamin\n\
\n\
:enum may take an optional argument to specify the quantity of output.\n\n" );

			else if ( !strcmp("ioops",yypvt[-0].up) )

fprintf( fpout, "\n\
IO operations\n\
\n\
:pr <filename>        Postfix operator to display automaton into a file\n\
:save <filename>      Postfix operator to save automaton in compressed form\n\
:load <filename>      Operator without left argument to get value from a file\n\
:readwords <filename> Operator with no argument to load a word file\n\
\n\
:get <variable>       Operator with no arguments to get value from a variable\n\
\n\
<var> = :load;        Short for <var> = :load <var>;\n\
:save <var>;          Short for <var> :save <var>;\n\n" );

			else if ( !strcmp("stats",yypvt[-0].up) )

fprintf( fpout, "\n\
Statement forms\n\
\n\
:bye;           Terminate session\n\
\n\
:alph;          Display token symbol table\n\
:free;          Display storage management report\n\
:list;          Display report lines for variables\n\
:noreport;      Turn off debug reporting\n\
:pr;            Save each variable into a file with the variable as file name\n\
:report;        Turn on debug reporting\n\
:save;          Save in compressed form all variables into files\n\
\n\
<var> = <exp>;  Assign value of expression to variable\n\
<exp>;          Compute and display expression\n\
<exp>:;         Compute and enumerate words in expression\n\
\n\
The action of <exp>; depends on the last operator evaluated:\n\
Coercing Operator:  display the value using :pr implicitly\n\
Printing Operator:  do not perform additional display\n\
Other:              coerce to DFAMIN and display using :pr\n\n" );

			else fprintf( fpout, "Unknown help request" );
		} else Warning( "Unknown function" );
		Sfree( yypvt[-1].up );
		Sfree( yypvt[-0].up );
		if ( A == NULL ) A = A_create();
		yyval.uA = A;
	} break;
case 13:
# line 493 "inrat.y"
{
		flag = 2;
	} break;
case 14:
# line 499 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(@)\n" );
		yyval.uA = A_compose( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 15:
# line 504 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(@@)\n" );
		yyval.uA = A_join( yypvt[-3].uA, yypvt[-0].uA );
	} break;
case 17:
# line 512 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "($)\n" );
		yyval.uA = A_retape( yypvt[-2].uA, yypvt[-0].uA, TT );
	} break;
case 19:
# line 520 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(|)\n" );
		yyval.uA = A_union( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 20:
# line 525 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(!)\n" );
		yyval.uA = A_xor( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 21:
# line 530 "inrat.y"
{
		fprintf( fpout, "Warning: Replace |! by ||\n" );
		if ( A_report ) fprintf( fpout, "(|!)\n" );
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
case 22:
# line 546 "inrat.y"
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
# line 561 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(!!)\n" );
		yyval.uA = A_shuffle( yypvt[-3].uA, yypvt[-0].uA );
	} break;
case 25:
# line 569 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(&)\n" );
		yyval.uA = A_intersect( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 26:
# line 574 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(-)\n" );
		yyval.uA = A_differ( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 28:
# line 582 "inrat.y"
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
# line 603 "inrat.y"
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
# line 624 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "( )\n" );
		yyval.uA = A_concat( yypvt[-1].uA, yypvt[-0].uA );
	} break;
case 34:
# line 632 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(+)\n" );
		yyval.uA = A_plus( yypvt[-1].uA );
	} break;
case 35:
# line 637 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(*)\n" );
		yyval.uA = A_star( yypvt[-1].uA );
	} break;
case 36:
# line 642 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(?)\n" );
		yyval.uA = A_opt( yypvt[-1].uA );
	} break;
case 37:
# line 647 "inrat.y"
{
		yyval.uA = A_lambda();
	} break;
case 38:
# line 651 "inrat.y"
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
# line 667 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "([])\n" );
		yyval.uA = A_comma( A_lambda(), yypvt[-1].uA );
	} break;
case 40:
# line 672 "inrat.y"
{
		yyval.uA = yypvt[-1].uA;
	} break;
case 41:
# line 676 "inrat.y"
{
		yyval.uA = A_lambda();
	} break;
case 42:
# line 680 "inrat.y"
{
		yyval.uA = yypvt[-1].uA;
	} break;
case 43:
# line 684 "inrat.y"
{
		yyval.uA = A_phi();
	} break;
case 44:
# line 690 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		yyval.uA = A_comma( yypvt[-2].uA, yypvt[-0].uA );
	} break;
case 45:
# line 695 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		yyval.uA = A_comma( A_lambda(), yypvt[-0].uA );
	} break;
case 46:
# line 700 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		yyval.uA = A_comma( yypvt[-1].uA, A_lambda() );
	} break;
case 47:
# line 705 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		yyval.uA = A_comma( A_lambda(), A_lambda() );
	} break;
case 49:
# line 713 "inrat.y"
{
		if ( A_report ) fprintf( fpout, "(,|)\n" );
		yyval.uA = A_union( yypvt[-2].uA, yypvt[-0].uA );
	} break;
		}
		goto yystack;  /* stack new state and value */

	}
