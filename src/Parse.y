/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 *
 * This software was developed while I was a student and, later, professor
 * at the University of Waterloo.  It has only minimal enhancements and bug
 * fixes from later than August 1988.  It was released under the GPLv3
 * licence on July 26, 2010.
 *                 -- J Howard Johnson ( j.howard.johnson *at* gmail.com )
 *
 * This file is part of INR.
 *
 *   INR is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   INR is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with INR.  If not, see <http://www.gnu.org/licenses/>.
 */

%{

#include "O.h"

    int             i, num;
    P_OBJECT P;
    Q_OBJECT Q;
    char *t;
    int l, i_var, tapeno, i_tok, i_aut;

#define PROMT   if(isatty(fileno(fpin))&&isatty(fileno(fpout)))printf("--* ");

    %}

%union {
    P_OBJECT        up;
    A_OBJECT        uA;
}
%type   <uA>    reg_0 reg_1 reg_2 reg_3 reg_4 reg_5 reg_6 reg_7 reg_8
%type   <uA>    paren_reg brace_reg
%type   <up>    name
%token          EXCLAM DOLLAR PERCENT   AMPERSAND
%token          LPAREN RPAREN STAR      PLUS       COMMA  MINUS  SLASH
%token          COLON  SEMI   EQUAL     QUESTION
%token          AT
%token          LBRACK BSLASH RBRACK    CIRCUMFLEX
%token          LBRACE VBAR   RBRACE
%token  <up>    LNAME PNAME

%%


statseq :
statseq statement
| statseq error SEMI
{
    yyerrok;
    if ( A_report ) pr_time_diff();
    PROMT
}
|
;

statement       :
reg_0 SEMI
{
    A = $1;
    if ( disp_flag == 2 ) A = A_min( A );
    if ( disp_flag ) {
        (void) A_rept( A );
        fprintf( fpout, "\n" );
        (void) A_pr( A, (char *) NULL, TT2 );
        fprintf( fpout, "\n" );
    }
    A_destroy( Alist[ 0 ] );
    Alist[ 0 ] = A;
    if ( A_report ) pr_time_diff();
    PROMT
}
| reg_0 COLON SEMI
{
    A = A_enum( $1, TT2, 1000 );
    A_destroy( Alist[ 0 ] );
    Alist[ 0 ] = A;
    if ( A_report ) pr_time_diff();
    PROMT
}
| name EQUAL reg_0 SEMI
{
    A = $3;
    if ( disp_flag == 2 ) A = A_min( A );
    if ( disp_flag ) {
        fprintf( fpout, "%s  ", pad20( P_cstr( $1 ) ) );
        (void) A_rept( A );
    }
    if ( (i = Tn_member( TAlist, P_cstr( $1 ), P_length( $1 ) )) >= 0 )
        A_destroy( Alist[ i ] );

    i_aut = Tn_insert( TAlist, P_cstr( $1 ), P_length( $1 ) );
    if ( i_aut >= Ssize( (char *) Alist ) / sizeof( A_OBJECT ) ) {
        Alist = (A_OBJECT *)
            Srealloc( (char *) Alist, 2 * i_aut * sizeof( A_OBJECT ) );
    }
    Alist[ i_aut ] = A;

    if ( T2_member( TT2, P_cstr( $1 ), P_length( $1 ) ) >= 0 )
        fprintf( fpout,
                 "Warning: %s is also a token\n", P_cstr( $1 ) );
    P_destroy( $1 );
    if ( A_report ) pr_time_diff();
    PROMT
}
| name EQUAL COLON name SEMI
{
    if ( !strcmp("read",P_cstr($4)) || !strcmp("load",P_cstr($4)) )
        A = A_load(P_cstr($1),TT2);
    else {
        Warning( "Unknown function" );
        A = A_create();
    }
    fprintf( fpout, "%s  ", pad20( P_cstr($1) ) );
    (void) A_rept( A );
    if ( (i = Tn_member( TAlist, P_cstr($1), P_length( $1 ) )) >= 0 )
        A_destroy( Alist[ i ] );

    i_aut = Tn_insert( TAlist, P_cstr( $1 ), P_length( $1 ) );
    if ( i_aut >= Ssize( (char *) Alist ) / sizeof( A_OBJECT ) ) {
        Alist = (A_OBJECT *)
            Srealloc( (char *) Alist, 2 * i_aut * sizeof( A_OBJECT ) );
    }
    Alist[ i_aut ] = A;

    if ( T2_member( TT2, P_cstr($1), P_length( $1 ) ) >= 0 )
        fprintf( fpout,
                 "Warning: %s is also a token\n", P_cstr($1) );
    P_destroy( $1 );
    P_destroy( $4 );
    if ( A_report ) pr_time_diff();
    PROMT
}
| COLON name SEMI
{
    i = do_n_i( P_cstr( $2 ) );
    P_destroy( $2 );
    if ( i ) return ( 0 );
    if ( A_report ) pr_time_diff();
    PROMT
}
;

reg_0   :
reg_0 COLON name
{
    $$ = do_an_a( $1, P_cstr( $3 ) );
    P_destroy( $3 );
}
| reg_0 COLON LPAREN name RPAREN
{
    disp_flag = 0;
    A = $1;
    if ( (num = tonum(P_cstr($4))) >= 0 ) {
        if ( A_report ) fprintf( fpout, "(@pow)\n" );
        A = A_cmpow( $1, num );
        disp_flag = 2;
    } else Warning( "Unknown function" );
    P_destroy( $4 );
    $$ = A;
}
| reg_0 COLON name name
{
    A = do_ann_a( $1, P_cstr($3), P_cstr($4) );
    P_destroy( $3 );
    P_destroy( $4 );
    $$ = A;
}
| COLON name name
{
    A = do_nn_a( P_cstr($2), P_cstr($3) );
    P_destroy( $2 );
    P_destroy( $3 );
    if ( A == NULL ) A = A_create();
    $$ = A;
}
| reg_1
{
    disp_flag = 2;
}
;

reg_1   :
reg_1 AT reg_2
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

reg_2   :
reg_2 DOLLAR reg_3
{
    if ( A_report ) fprintf( fpout, "($)\n" );
    $$ = A_retape( $1, $3, TT2 );
}
| reg_2 PERCENT reg_3
{
    if ( A_report ) fprintf( fpout, "(%%)\n" );
    $$ = A_percent( $1, $3 );
}
| reg_3
;

reg_3   :
reg_3 VBAR reg_4
{
    if ( A_report ) fprintf( fpout, "(|)\n" );
    $$ = A_union( $1, $3 );
}
| reg_3 EXCLAM reg_4
{
    if ( A_report ) fprintf( fpout, "(!)\n" );
    $$ = A_xor( $1, $3 );
}
| reg_3 VBAR VBAR reg_4
{
    if ( A_report ) fprintf( fpout, "(||)\n" );
    if ( $1-> A_nT > 1 || $4-> A_nT > 1 ) {
        Atemp = A_differ(
            A_retape( A_copy( $4 ),
                      A_letter( 0, T2_insert( TT2, "0", 1 ) ), TT2
                    ),
            A_retape( A_copy( $1 ),
                      A_letter( 0, T2_insert( TT2, "0", 1 ) ), TT2
                    )
        );
        $$ = A_union( $1, A_join( Atemp, $4 ) );
    } else  $$ = A_union( $1, $4 );
}
| reg_3 EXCLAM EXCLAM reg_4
{
    if ( A_report ) fprintf( fpout, "(!!)\n" );
    $$ = A_shuffle( $1, $4 );
}
| reg_4
;

reg_4   :
reg_4 AMPERSAND reg_5
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

reg_5   :
reg_5 BSLASH reg_6
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
                              A_letter( 0, T2_insert( TT2, "0", 1 ) ),
                              A_letter( 0, T2_insert( TT2, "0", 1 ) ) ),
                          TT2 );
        $$ = A_compose( $3, A_concat( $1, Atemp ) );
    }
}
| reg_6
;

reg_6   :
reg_6 SLASH reg_7
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
                              A_letter( 0, T2_insert( TT2, "0", 1 ) ),
                              A_letter( 0, T2_insert( TT2, "0", 1 ) ) ),
                          TT2);
        $$ = A_compose( $1, A_concat( Atemp, $3 ) );
    }
}
| reg_7
;

reg_7   :
reg_7 reg_8
{
    if ( A_report ) fprintf( fpout, "( )\n" );
    $$ = A_concat( $1, $2 );
}
| reg_8
;

reg_8   :
reg_8 PLUS
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
| LNAME
{
    t = P_cstr( $1 );
    if ( (i = Tn_member( TAlist, t, P_length( $1 ) )) >= 0
              && T2_member( TT2, t, P_length( $1 ) ) < 0 )
        $$ = A_copy( Alist[ i ] );
    else {
        $$ = A_letter( 0, T2_insert( TT2, t, P_length( $1 ) ) );
        if ( i >= 0 )
            fprintf( fpout,
                     "Warning: %s is a variable and a token\n", t );
    }
    P_destroy( $1 );
}
| PNAME
{
    P = $1;
    t = P_cstr( P );
    l = P_length( P );
    i_var = Tn_member( TAlist, t, l );

    Q = Q_fromP( P );
    t = Q_cstr( Q );
    l = Q_length( Q );
    tapeno = Q_tapeno( Q );
    i_tok = T2_member( TT2, t, l );

    if ( i_var >= 0 && i_tok >= 0 ) {
        fprintf( fpout, "Warning: %s is a variable and a token\n", t );
    }

    if ( i_var < 0 ) {
        if ( tapeno < 0 ) { tapeno = 0; }
        i_tok = T2_insert( TT2, t, l );
        $$ = A_letter( tapeno, i_tok );
    } else {
        $$ = A_copy( Alist[ i_var ] );
    }
    Q_destroy( Q );

/*
    t = P_cstr( $1 );
    if ( t[1] == '.' && t[0] >= '0' && t[0] <= '9' )
        $$ = A_letter( t[0] - '0',
            T2_insert( TT2, t + 2, P_length( $1 ) - 2 ) );
    else if ( (i = Tn_member( TAlist, t, P_length( $1 ) )) >= 0
              && T2_member( TT2, t, P_length( $1 ) ) < 0 )
        $$ = A_copy( Alist[ i ] );
    else {
        $$ = A_letter( 0, T2_insert( TT2, t, P_length( $1 ) ) );
        if ( i >= 0 )
            fprintf( fpout,
                     "Warning: %s is a variable and a token\n", t );
    }
    P_destroy( $1 );
*/
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

paren_reg :
paren_reg COMMA reg_0
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

brace_reg :
brace_reg COMMA reg_0
{
    if ( A_report ) fprintf( fpout, "(,|)\n" );
    $$ = A_union( $1, $3 );
}
| reg_0
;

name :
LNAME
| PNAME
;

%%
