/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Not for sale or distribution without written permission of the author.
 */

%{

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

%}

%union {
	char *		up;
	A_OBJECT	uA;
}
%type	<uA>	reg_0 reg_1 reg_2 reg_3 reg_4 reg_5 reg_6 reg_7 reg_8
%type	<uA>	paren_reg brace_reg
%token		EXCLAM DOLLAR AMPERSAND
%token		LPAREN RPAREN STAR      PLUS       COMMA  MINUS  SLASH
%token		COLON  SEMI   EQUAL     QUESTION
%token		AT
%token		LBRACK BSLASH RBRACK    CIRCUMFLEX
%token		LBRACE VBAR   RBRACE
%token	<up>	NAME

%%

statseq	: statseq statement
	| statseq error SEMI
	{
		yyerrok;
		if ( A_report ) pr_time_diff();
		PROMT
	}
	|
	;

statement	: reg_0 SEMI
		{
			A = $1;
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
		}
		| reg_0 COLON SEMI
		{
			A = A_enum( $1, TT, 1000 );
			A_destroy( Alist[ 0 ] );
			Alist[ 0 ] = A;
			if ( A_report ) pr_time_diff();
			PROMT
		}
		| NAME EQUAL reg_0 SEMI
		{
			A = $3;
			if ( flag == 2 ) A = A_min( A );
			if ( flag ) {
				fprintf( fpout, "%s  ", pad20( $1 ) );
				(void) A_rept( A );
			}
			if ( (i = T_member( TAlist, $1 )) >= 0 )
				A_destroy( Alist[ i ] );
			Alist[ T_insert( TAlist, $1 ) ] = A;
			if ( T_member( TT, $1 ) >= 0 )
				fprintf( fpout,
					"Warning: %s is also a token\n", $1 );
			Sfree( $1 );
			if ( A_report ) pr_time_diff();
			PROMT
		}
		| NAME EQUAL COLON NAME SEMI
		{
			if ( !strcmp("read",$4) || !strcmp("load",$4) )
				A = A_load($1,TT);
			else {
				Warning( "Unknown function" );
				A = A_create();
			}
			fprintf( fpout, "%s  ", pad20( $1 ) );
			(void) A_rept( A );
			if ( (i = T_member( TAlist, $1 )) >= 0 )
				A_destroy( Alist[ i ] );
			Alist[ T_insert( TAlist, $1 ) ] = A;
			if ( T_member( TT, $1 ) >= 0 )
				fprintf( fpout,
					"Warning: %s is also a token\n", $1 );
			Sfree( $1 );
			Sfree( $4 );
			if ( A_report ) pr_time_diff();
			PROMT
		}
		| COLON NAME SEMI
		{
			if ( !strcmp("alph",$2) ) {
				fprintf( fpout, "\n" );
				for( i = 2; i < TT-> T_n; i++ ) {
					fprintf( fpout, T_name( TT, i ) );
					fprintf( fpout, " " );
				}
				fprintf( fpout, "\n" );
			} else if ( !strcmp("free",$2) ) {
				fprintf( fpout, "\n" );
				(void) Sarena();
				fprintf( fpout, "\n" );
			} else if ( !strcmp("list",$2) ) {
				fprintf( fpout, "\n" );
				for( i = 0; i < TAlist-> T_n; i++ ) {
					fprintf( fpout, "%s  ", pad20(
						T_name( TAlist, i ) ) );
					(void) A_rept( Alist[ i ] );
				}
				fprintf( fpout, "\n" );
			} else if ( !strcmp("noreport",$2) ) A_report = 0;
			else if ( !strcmp("pr",$2) ) {
				for( i = 1; i < TAlist-> T_n; i++ )
					if ( Alist[ i ]-> A_nrows > 0 )
					(void) A_store( Alist[ i ],
						T_name( TAlist, i ), TT );
			} else if ( !strcmp("report",$2) ) A_report = 1;
			else if ( !strcmp("save",$2) ) {
				for( i = 1; i < TAlist-> T_n; i++ )
					if ( Alist[ i ]-> A_nrows > 0 )
					Alist[ i ] = A_save( Alist[ i ],
						T_name( TAlist, i ), TT );
			} else if ( !strcmp("time",$2) ) pr_time_diff();
			else if( !strcmp("help",$2) )

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

			else if (  !strcmp("done",$2)
				|| !strcmp("quit",$2)
				|| !strcmp("stop",$2)
				|| !strcmp("bye",$2) ) return;
			else Warning( "Unknown function" );
			Sfree( $2 );
			if ( A_report ) pr_time_diff();
			PROMT
		}
		;

reg_0	: reg_0 COLON NAME
	{
		flag = 0;
		A = $1;
		if ( !strcmp("pr",$3) ) {
			(void) A_rept( $1 );
			fprintf( fpout, "\n" );
			A = A_store( $1, (char *) NULL, TT );
			fprintf( fpout, "\n" );
		} else if ( !strcmp("deecho",$3) ) {
			A = $1;
			A = A_deecho( A,
				T_insert(TT,"ECHO") * A-> A_nT + 1,
				T_insert(TT,"NOECHO") * A-> A_nT + 1 );
			flag = 2;
		} else if ( !strcmp("prsseq",$3) ) {
			fprintf( fpout, "\n" );
			A = A_prsseq( $1, (char *) NULL, TT );
			fprintf( fpout, "\n" );
		} else if ( !strcmp("acomp",$3) ) {
			if ( A_report ) fprintf( fpout, "(acomp)\n" );
			Atemp = A_star( A_alph( A_copy( $1 ) ) );
			A = A_differ( Atemp, $1 );
			flag = 2;
		} else if ( !strcmp("alph",$3) ) {
			if ( A_report ) fprintf( fpout, "(alph)\n" );
			A = A_alph( $1 );
			flag = 2;
		} else if ( !strcmp("card",$3) ) {
			i = A_card( $1 );
			if ( i < 0 ) fprintf( fpout, "Infinite\n" );
			else {
				fprintf( fpout, "%d word", i );
				if ( i != 1 ) fprintf( fpout, "s" );
				fprintf( fpout, "\n" );
			}
		} else if ( !strcmp("closed",$3) ) {
			A = A_clsure( $1 );
			flag = 1;
		} else if ( !strcmp("clsseq",$3) ) {
			A = A_clsseq( $1 );
			flag = 2;
		} else if ( !strcmp("comp",$3) ) {
			if ( A_report ) fprintf( fpout, "(comp)\n" );
			if ( (i = T_member( TAlist, "SIGMA" )) >= 0 ) {
				Atemp = A_star( A_alph( A_copy( Alist[i] ) ) );
				A = A_differ( Atemp, $1 );
			} else  fprintf( fpout,
					"Error in comp: SIGMA not defined\n" );
			flag = 2;
		} else if ( !strcmp("dfa",$3) ) {
			A = A_subs( $1 );
			flag = 1;
		} else if ( !strcmp("dfamin",$3) ) {
			A = A_min( $1 );
			flag = 1;
		} else if ( !strcmp("enum",$3) ) {
			A = A_enum( $1, TT, 1000 );
		} else if ( !strcmp("limit",$3) ) {
			if ( A_report ) fprintf( fpout, "(limit)\n" );
			if ( $1-> A_nT != 2 )
				Error( "Wrong number of tapes (limit)" );

		Atemp = A_differ(
				A_retape(
					A_copy( $1 ),
					A_letter( 0, T_insert( TT, "1" ) ),
					TT
				),
				A_retape(
					A_copy( $1 ),
					A_letter( 0, T_insert( TT, "0" ) ),
					TT
				)
		);

			A = A_join( $1, Atemp );
			flag = 2;
		} else if ( !strcmp("lamcm",$3) ) {
			A = A_lamcm( $1 );
			flag = 1;
		} else if ( !strcmp("lameq",$3) ) {
			A = A_lameq( $1 );
			flag = 1;
		} else if ( !strcmp("lenmin",$3) ) {
			A = A_lenmin( $1 );
			flag = 2;
		} else if ( !strcmp("min",$3) ) {
			A = A_min( $1 );
			flag = 1;
		} else if ( !strcmp("length",$3) ) {
			i = A_minlen( $1 );
			A = $1;
			if ( i < 0 ) fprintf( fpout, "Empty Set\n" );
			else {
				fprintf( fpout, "%d letter", i );
				if ( i != 1 ) fprintf( fpout, "s" );
				fprintf( fpout, "\n" );
			}
		} else if ( !strcmp("nfa",$3) ) {
			A = A_close( $1 );
			flag = 1;
		} else if ( !strcmp("pref",$3) ) {
			if ( A_report ) fprintf( fpout, "(pref)\n" );
			A = A_pref( $1 );
			flag = 2;
		} else if ( !strcmp("report",$3) ) {
			A = A_rept( $1 );
		} else if ( !strcmp("rev",$3) ) {
			if ( A_report ) fprintf( fpout, "(rev)\n" );
			A = A_rev( $1 );
			flag = 2;
		} else if ( !strcmp("sseq",$3) ) {
			A = A_sseq( $1 );
			flag = 2;
		} else if ( !strcmp("LMsseq",$3) ) {
			A = A_LMsseq( $1 );
			flag = 2;
		} else if ( !strcmp("GMsseq",$3) ) {
			A = A_GMsseq( $1 );
			flag = 2;
		} else if ( !strcmp("stems",$3) ) {
			A_prstems( $1, TT, 0 );
		} else if ( !strcmp("suff",$3) ) {
			if ( A_report ) fprintf( fpout, "(suff)\n" );
			A = A_suff( $1 );
			flag = 2;
		} else if ( !strcmp("time",$3) ) {
			pr_time_diff();
			flag = 2;
		} else if ( !strcmp("trim",$3) ) {
			A = A_trim( $1 );
			flag = 1;
		} else if ( !strcmp("update",$3) ) {
			A = A_open( $1 );
			flag = 1;
		} else if ( (num = tonum($3)) >= 0 ) {
			if ( A_report ) fprintf( fpout, "(pow)\n" );
			A = A_catpow( $1, num );
			flag = 2;
		} else  Warning( "Unknown function" );
		Sfree( $3 );
		$$ = A;
	}
	| reg_0 COLON LPAREN NAME RPAREN
	{
		flag = 0;
		A = $1;
		if ( (num = tonum($4)) >= 0 ) {
			if ( A_report ) fprintf( fpout, "(@pow)\n" );
			A = A_cmpow( $1, num );
			flag = 2;
		} else Warning( "Unknown function" );
		Sfree( $4 );
		$$ = A;
	}
	| reg_0 COLON NAME NAME
	{
		flag = 0;
		A = $1;
		if ( !strcmp("enum",$3) && (num = tonum($4)) >= 0)
			A = A_enum( $1, TT, num );
		else if ( !strcmp("save",$3) )
			A = A_save( $1, $4, TT );
		else if ( !strcmp("pr",$3) ) 
			A = A_store( $1, $4, TT );
		else if ( !strcmp("prsseq",$3) ) 
			A = A_prsseq( $1, $4, TT );
		else if ( !strcmp("stems",$3) )
			A_prstems( $1, TT, tonum($4) );
		else if ( !strcmp("surgery",$3) ) {
			num = tonum($4);
			if ( num < 2 || num >= $1-> A_nrows )
				Warning( "Illegal surgery" );
			else {
				A = A_open( $1 );
				A = A_add( A, num,
					T_insert(TT,"INCISION") * A-> A_nT,
					num );
			}
		} else Warning( "Unknown function" );
		Sfree( $3 );
		Sfree( $4 );
		$$ = A;
	}
	| COLON NAME NAME
	{
		flag = 0;
		A = NULL;
		if ( !strcmp("get",$2) ) {
			if ( (i = T_member( TAlist, $3 )) >= 0 )
				A = A_copy( Alist[i] );
			flag = 1;
		} else if ( !strcmp("read",$2) || !strcmp("load",$2) ) {
			A = A_load( $3, TT );
			flag = 1;
		} else if ( !strcmp("readwords",$2) ) {
			A = A_lwds( $3, TT );
			flag = 1;
		} else if ( !strcmp("save",$2) ) {
			if ( (i = T_member( TAlist, $3 )) >= 0 )
				A = A_save( A_copy( Alist[i] ), $3, TT );
			else fprintf( fpout, "Warning: %s undefined\n", $3 );
		} else if ( !strcmp("pr",$2) ) {
			if ( (i = T_member( TAlist, $3 )) >= 0 )
				A = A_store( Alist[i], $3, TT );
			else fprintf( fpout, "Warning: %s undefined\n", $3 );
		} else if ( !strcmp("help",$2) ) {
			if ( !strcmp("ops",$3) )

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

			else if ( !strcmp("colonops",$3) )

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

			else if ( !strcmp("ioops",$3) )

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

			else if ( !strcmp("stats",$3) )

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
		Sfree( $2 );
		Sfree( $3 );
		if ( A == NULL ) A = A_create();
		$$ = A;
	}
	| reg_1
	{
		flag = 2;
	}
	;

reg_1	: reg_1 AT reg_2
	{
		if ( A_report ) fprintf( fpout, "(@)\n" );
		$$ = A_compose( $1, $3 );
	}
	| reg_1 AT AT reg_2
	{
		if ( A_report ) fprintf( fpout, "(@@)\n" );
		$$ = A_join( $1, $4 );
	}
	| reg_2
	;

reg_2	: reg_2 DOLLAR reg_3
	{
		if ( A_report ) fprintf( fpout, "($)\n" );
		$$ = A_retape( $1, $3, TT );
	}
	| reg_3
	;

reg_3	: reg_3 VBAR reg_4
	{
		if ( A_report ) fprintf( fpout, "(|)\n" );
		$$ = A_union( $1, $3 );
	}
	| reg_3 EXCLAM reg_4
	{
		if ( A_report ) fprintf( fpout, "(!)\n" );
		$$ = A_xor( $1, $3 );
	}
	| reg_3 VBAR EXCLAM reg_4
	{
		fprintf( fpout, "Warning: Replace |! by ||\n" );
		if ( A_report ) fprintf( fpout, "(|!)\n" );
		if ( $1-> A_nT > 1 || $4-> A_nT > 1 ) {
			Atemp = A_differ(
					A_retape( A_copy( $4 ),
					A_letter( 0, T_insert( TT, "0" ) ), TT
						),
					A_retape( A_copy( $1 ),
					A_letter( 0, T_insert( TT, "0" ) ), TT
						)
			);
			$$ = A_union( $1, A_join( Atemp, $4 ) );
		} else	$$ = A_union( $1, $4 );
	}
	| reg_3 VBAR VBAR reg_4
	{
		if ( A_report ) fprintf( fpout, "(||)\n" );
		if ( $1-> A_nT > 1 || $4-> A_nT > 1 ) {
			Atemp = A_differ(
					A_retape( A_copy( $4 ),
					A_letter( 0, T_insert( TT, "0" ) ), TT
						),
					A_retape( A_copy( $1 ),
					A_letter( 0, T_insert( TT, "0" ) ), TT
						)
			);
			$$ = A_union( $1, A_join( Atemp, $4 ) );
		} else	$$ = A_union( $1, $4 );
	}
	| reg_3 EXCLAM EXCLAM reg_4
	{
		if ( A_report ) fprintf( fpout, "(!!)\n" );
		$$ = A_shuffle( $1, $4 );
	}
	| reg_4
	;

reg_4	: reg_4 AMPERSAND reg_5
	{
		if ( A_report ) fprintf( fpout, "(&)\n" );
		$$ = A_intersect( $1, $3 );
	}
	| reg_4 MINUS reg_5
	{
		if ( A_report ) fprintf( fpout, "(-)\n" );
		$$ = A_differ( $1, $3 );
	}
	| reg_5
	;

reg_5	: reg_5 BSLASH reg_6
	{
		if ( A_report ) fprintf( fpout, "(\\)\n" );
		if ( $1-> A_nT > 1 || $3-> A_nT > 1 ) {
			fprintf( fpout,
				"Error: \\ applied to multitape automaton\n" );
			A_destroy( $1 );
			A_destroy( $3 );
			$$ = A_phi();
		} else {
			Atemp = A_retape( A_star( A_alph( A_copy( $3 ) ) ),
				A_comma(
					A_letter( 0, T_insert( TT, "0" ) ),
					A_letter( 0, T_insert( TT, "0" ) ) ),
				TT );
			$$ = A_compose( $3, A_concat( $1, Atemp ) );
		}
	}
	| reg_6
	;

reg_6	: reg_6 SLASH reg_7
	{
		if ( A_report ) fprintf( fpout, "(/)\n" );
		if ( $1-> A_nT > 1 || $3-> A_nT > 1 ) {
			fprintf( fpout,
				"Error: / applied to multitape automaton\n" );
			A_destroy( $1 );
			A_destroy( $3 );
			$$ = A_phi();
		} else {
			Atemp = A_retape( A_star( A_alph( A_copy( $1 ) ) ),
				A_comma(
					A_letter( 0, T_insert( TT, "0" ) ),
					A_letter( 0, T_insert( TT, "0" ) ) ),
				TT );
			$$ = A_compose( $1, A_concat( Atemp, $3 ) );
		}
	}
	| reg_7
	;

reg_7	: reg_7 reg_8
	{
		if ( A_report ) fprintf( fpout, "( )\n" );
		$$ = A_concat( $1, $2 );
	}
	| reg_8
	;

reg_8	: reg_8 PLUS
	{
		if ( A_report ) fprintf( fpout, "(+)\n" );
		$$ = A_plus( $1 );
	}
	| reg_8 STAR
	{
		if ( A_report ) fprintf( fpout, "(*)\n" );
		$$ = A_star( $1 );
	}
	| reg_8 QUESTION
	{
		if ( A_report ) fprintf( fpout, "(?)\n" );
		$$ = A_opt( $1 );
	}
	| CIRCUMFLEX
	{
		$$ = A_lambda();
	}
	| NAME
	{
		t = $1;
		if ( t[1] == '.' && t[0] >= '0' && t[0] <= '9' )
			$$ = A_letter( t[0] - '0', T_insert( TT, t + 2 ) );
		else if ( (i = T_member( TAlist, t )) >= 0
			&& T_member( TT, t ) < 0 )
				$$ = A_copy( Alist[ i ] );
		else {
			$$ = A_letter( 0, T_insert( TT, t ) );
			if ( i >= 0 )
			fprintf( fpout,
				"Warning: %s is a variable and a token\n", t );
		}
		Sfree( $1 );
	}
	| LBRACK reg_0 RBRACK
	{
		if ( A_report ) fprintf( fpout, "([])\n" );
		$$ = A_comma( A_lambda(), $2 );
	}
	| LPAREN paren_reg RPAREN
	{
		$$ = $2;
	}
	| LPAREN RPAREN
	{
		$$ = A_lambda();
	}
	| LBRACE brace_reg RBRACE
	{
		$$ = $2;
	}
	| LBRACE RBRACE
	{
		$$ = A_phi();
	}
	;

paren_reg : paren_reg COMMA reg_0
	{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		$$ = A_comma( $1, $3 );
	}
	| COMMA reg_0
	{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		$$ = A_comma( A_lambda(), $2 );
	}
	| paren_reg COMMA
	{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		$$ = A_comma( $1, A_lambda() );
	}
	| COMMA
	{
		if ( A_report ) fprintf( fpout, "(,)\n" );
		$$ = A_comma( A_lambda(), A_lambda() );
	}
	| reg_0
	;

brace_reg : brace_reg COMMA reg_0
	{
		if ( A_report ) fprintf( fpout, "(,|)\n" );
		$$ = A_union( $1, $3 );
	}
	| reg_0
	;

%%

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
