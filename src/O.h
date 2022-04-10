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

#include  "local.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

/* this seems to be needed with the -ansi option to suppress warnings */
int fileno( FILE * );

#define Tn_Object       1
#define V_Object        2
#define R_Object        3
#define U_Object        4
#define A_Object        5
#define P_Object        6
#define Q_Object        7
#define T2_Object       8

typedef int             SHORT;
#define MAXSHORT        017777777777

typedef struct {
    SHORT           R_a;
    SHORT           R_b;
}       R_row;

typedef struct {
    SHORT           A_a;
    SHORT           A_b;
    SHORT           A_c;
}       A_row;

typedef struct Tn_desc {
    int             Type;
    int             Tn_n;
    int             Tn_lname;
    int             Tn_lhash;
    int             Tn_lstor;
    int   *         Tn_idxs;
    SHORT *         Tn_hash;
    char  *         Tn_stor;
} *     Tn_OBJECT;

typedef struct T2_desc {
    int             Type;
    Tn_OBJECT       T2_int;
    Tn_OBJECT       T2_ext;
} *     T2_OBJECT;

typedef struct V_desc {
    int             Type;
    int             V_n;
    int             V_lvec;
    int             V_lhash;
    SHORT **        V_vec;
    SHORT *         V_hash;
} *     V_OBJECT;

typedef struct R_desc {
    int             Type;
    int             R_n;
    int             R_lrec;
    int             R_lhash;
    R_row *         R_rec;
    SHORT *         R_hash;
} *     R_OBJECT;

typedef struct U_desc {
    int             Type;
    int             U_n;
    int             U_lrec;
    int             U_lhash;
    A_row *         U_rec;
    SHORT *         U_hash;
} *     U_OBJECT;

typedef struct A_desc {
    int             Type;
    SHORT           A_mode;
    SHORT           A_ems;
    int             A_nT;
    int             A_nQ;
    int             A_nS;
    int             A_nrows;
    int             A_lrows;
    A_row **        A_p;
    A_row *         A_t;
} *     A_OBJECT;

typedef struct P_desc {
    int             Type;
    int             P_length;
    char *          P_cstr;
} *     P_OBJECT;

typedef struct Q_desc {
    int             Type;
    int             Q_tapeno;
    int             Q_length;
    char *          Q_cstr;
} *     Q_OBJECT;

#define OPEN            0
#define NFA             1
#define NFA_TRIM        2
#define NFA_EQLAM       3
#define NFA_CMLAM       4
#define NFA_CLOSED      5
#define DFA             6
#define DFA_MIN         7
#define SSEQ            8
#define SSEQ_MIN        9

#define START           0
#define FINAL           1

extern FILE *fpin;
extern FILE *fpout;

/* S.c */
void        copymem( long, char *, char * );
char *      Salloc( long );
void        Sfree( char * );
char *      Srealloc( char *, long );
char *      Scopy( char * );
long        Ssize( char * );
char *      Sfind( char * );
void        Sarena();
void        Saudit();

/* stamp.c */
extern char Version[];
extern char Date[];

/* util.c */
void        Warning( char * );
void        Error( char * );
SHORT       *s_alloc( int );
int         *i_alloc( int );
void        pr_time_diff();

/* Tn.c */
Tn_OBJECT   Tn_create();
void        Tn_destroy( Tn_OBJECT );
int         Tn_member( Tn_OBJECT, char *, int );
Tn_OBJECT   Tn_grow( Tn_OBJECT, int );
int         Tn_insert( Tn_OBJECT, char *, int );
char *      Tn_name( Tn_OBJECT, int );
int         Tn_length( Tn_OBJECT, int );
P_OBJECT    Tn_Pstr( Tn_OBJECT, int );
void        Tn_stats();

/* T2.c */
T2_OBJECT   T2_create();
void        T2_destroy( T2_OBJECT );
int         T2_member( T2_OBJECT, char *, int );
int         T2_insert( T2_OBJECT, char *, int );
char *      T2_name( T2_OBJECT, int );
int         T2_length( T2_OBJECT, int );
P_OBJECT    T2_Pstr( T2_OBJECT, int );
char *      T2_name_pr( T2_OBJECT, int );
int         T2_length_pr( T2_OBJECT, int );
P_OBJECT    T2_Pstr_pr( T2_OBJECT, int );
void        T2_stats();
void        T2_sync( T2_OBJECT );

/* P.c */
P_OBJECT    P_create( int, char * );
P_OBJECT    P_fromQ( Q_OBJECT );
void        P_destroy( P_OBJECT );
int         P_length( P_OBJECT );
char *      P_cstr( P_OBJECT );

/* Q.c */
Q_OBJECT    Q_create( int, int, char * );
Q_OBJECT    Q_fromP( P_OBJECT );
void        Q_destroy( Q_OBJECT );
int         Q_tapeno( Q_OBJECT );
int         Q_length( Q_OBJECT );
char *      Q_cstr( Q_OBJECT );

/* V.c */
SHORT *     veccpy( SHORT *, SHORT * );
int         veccmp( SHORT *, SHORT * );
int         veclen( SHORT * );

V_OBJECT    V_create();
void        V_destroy( V_OBJECT );
int         V_member( V_OBJECT, SHORT * );
V_OBJECT    V_grow( V_OBJECT, int );
int         V_insert( V_OBJECT, SHORT * );
SHORT *     V_vec( V_OBJECT, int );
void        V_stats();

/* R.c */
R_OBJECT    R_create();
void        R_destroy( R_OBJECT );
int         R_member( R_OBJECT, int, int );
R_OBJECT    R_grow( R_OBJECT, int );
int         R_insert( R_OBJECT, int, int );
R_row *     R_rec( R_OBJECT, int );
void        R_stats();

/* U.c */
U_OBJECT    U_create();
void        U_destroy( U_OBJECT );
int         U_member( U_OBJECT, int, int, int );
U_OBJECT    U_grow( U_OBJECT, int );
int         U_insert( U_OBJECT, int, int, int );
A_row *     U_rec( U_OBJECT, int );
void        U_stats();

/* Acrea.c */
extern int  A_report;
A_OBJECT    A_create();
void        A_destroy( A_OBJECT );
A_OBJECT    A_rept( A_OBJECT );
void        A_exchange( A_OBJECT, A_OBJECT );
A_OBJECT    A_copy( A_OBJECT );
A_OBJECT    A_deems( A_OBJECT );
A_OBJECT    A_adems( A_OBJECT );

/* Aopen.c */
extern SHORT * s_rena;
extern int     f_rena;
A_OBJECT    A_add( A_OBJECT, int, int, int );
A_OBJECT    A_open( A_OBJECT );
A_OBJECT    A_close( A_OBJECT );
A_OBJECT    A_rename( A_OBJECT, SHORT * );
A_OBJECT    A_mkdense( A_OBJECT );

/* Aload.c */
A_OBJECT    A_load( char *, T2_OBJECT );
A_OBJECT    A_lwds( char *, T2_OBJECT );
A_OBJECT    A_prsseq( A_OBJECT, char *, T2_OBJECT );

/* Apr.c */
A_OBJECT    A_pr( A_OBJECT, char *, T2_OBJECT );
A_OBJECT    A_load_pr( char *, T2_OBJECT );

/* Asave.c */
A_OBJECT    A_save( A_OBJECT, char *, T2_OBJECT );
A_OBJECT    A_load_save( char *, T2_OBJECT );

/* Atrim.c */
A_OBJECT    A_trim( A_OBJECT );

/* Alameq.c */
A_OBJECT    A_lameq( A_OBJECT );

/* Alamcm.c */
A_OBJECT    A_lamcm( A_OBJECT );

/* Aclos.c */
A_OBJECT    A_clsure( A_OBJECT );

/* Asubs.c */
A_OBJECT    A_subs( A_OBJECT );

/* Amin.c */
A_OBJECT    A_min( A_OBJECT );

/* Abasic.c */
void        A_conform( A_OBJECT, A_OBJECT );
A_OBJECT    A_phi();
A_OBJECT    A_lambda();
A_OBJECT    A_letter( int, int);
A_OBJECT    A_deecho( A_OBJECT, int, int );
A_OBJECT    A_opt( A_OBJECT );
A_OBJECT    A_plus( A_OBJECT );
A_OBJECT    A_star( A_OBJECT );
A_OBJECT    A_union( A_OBJECT, A_OBJECT );
A_OBJECT    A_percent( A_OBJECT, A_OBJECT );
A_OBJECT    A_concat( A_OBJECT, A_OBJECT );
A_OBJECT    A_intersect( A_OBJECT, A_OBJECT );
A_OBJECT    A_differ( A_OBJECT, A_OBJECT );
A_OBJECT    A_xor( A_OBJECT, A_OBJECT );
A_OBJECT    A_alph( A_OBJECT );
A_OBJECT    A_rev( A_OBJECT );
A_OBJECT    A_shuffle( A_OBJECT, A_OBJECT );

/* Aenum.c */
A_OBJECT    A_enum( A_OBJECT, T2_OBJECT, int );
int         A_card( A_OBJECT );
A_OBJECT    A_pref( A_OBJECT );
A_OBJECT    A_suff( A_OBJECT );

/* Acompose.c */
A_OBJECT    A_compose( A_OBJECT, A_OBJECT );

/* Ajoin.c */
A_OBJECT    A_join( A_OBJECT, A_OBJECT );

/* Aretape.c */
A_OBJECT    A_retape( A_OBJECT, A_OBJECT, T2_OBJECT );
A_OBJECT    A_comma( A_OBJECT, A_OBJECT );

/* Apowers.c */
A_OBJECT    A_catpow( A_OBJECT, int );
A_OBJECT    A_ident( A_OBJECT );
A_OBJECT    A_cmpow( A_OBJECT, int );

/* Alenmin.c */
A_OBJECT    A_lenmin( A_OBJECT );
int         A_minlen( A_OBJECT );

/* Astems.c */
void        A_st_free();
SHORT **    A_stems( A_OBJECT, int );
void        A_prstems( A_OBJECT, T2_OBJECT, int );

/* Asseq.c */
A_OBJECT    A_sseq( A_OBJECT );

/* ALMsseq.c */
A_OBJECT    A_LMsseq( A_OBJECT );

/* AGMsseq.c */
A_OBJECT    A_GMsseq( A_OBJECT );

/* Aclsseq.c */
A_OBJECT    A_clsseq( A_OBJECT );

/* Colon.c */
extern int  disp_flag;
int         do_n_i( char * );
A_OBJECT    do_an_a( A_OBJECT, char * );
A_OBJECT    do_ann_a( A_OBJECT, char *, char * );
A_OBJECT    do_nn_a( char *, char * );

/* Lex.c */
extern A_OBJECT A;
extern A_OBJECT Atemp;
extern A_OBJECT Alist[1000];
extern Tn_OBJECT TAlist;

extern T2_OBJECT TT2;

extern char Notice[];
char *      pad20( char * );
int         yylex();
void        yyerror( char * );
int         tonum( char * );

/* Parse.y */
int         yyparse( void );

/* Aunicode.c */
A_OBJECT    A_slurp_octets( char *, T2_OBJECT );
A_OBJECT    A_slurp_utf8( char *, T2_OBJECT );
A_OBJECT    A_spit_octets( A_OBJECT, char *, T2_OBJECT );
A_OBJECT    A_spit_utf8( A_OBJECT, char *, T2_OBJECT );
A_OBJECT    A_gen_min( A_OBJECT );
int         A_valid_utf8_at( char *, int, int, int * );
int         A_unicode_printable( int );
