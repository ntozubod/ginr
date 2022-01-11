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
#define T_Object        1
#define V_Object        2
#define R_Object        3
#define U_Object        4
#define A_Object        5
typedef unsigned short SHORT ;
#define MAXSHORT        0177777
typedef struct {
  SHORT R_a ;
  SHORT R_b ;
}
R_row ;
typedef struct {
  SHORT A_a ;
  SHORT A_b ;
  SHORT A_c ;
}
A_row ;
typedef struct T_desc {
  int Type ;
  int T_n ;
  int T_lname ;
  int T_lhash ;
  char ** T_name ;
  SHORT * T_hash ;
}
* T_OBJECT ;
typedef struct V_desc {
  int Type ;
  int V_n ;
  int V_lvec ;
  int V_lhash ;
  SHORT ** V_vec ;
  SHORT * V_hash ;
}
* V_OBJECT ;
typedef struct R_desc {
  int Type ;
  int R_n ;
  int R_lrec ;
  int R_lhash ;
  R_row * R_rec ;
  SHORT * R_hash ;
}
* R_OBJECT ;
typedef struct U_desc {
  int Type ;
  int U_n ;
  int U_lrec ;
  int U_lhash ;
  A_row * U_rec ;
  SHORT * U_hash ;
}
* U_OBJECT ;
typedef struct A_desc {
  int Type ;
  SHORT A_mode ;
  SHORT A_ems ;
  int A_nT ;
  int A_nQ ;
  int A_nS ;
  int A_nrows ;
  int A_lrows ;
  A_row ** A_p ;
  A_row * A_t ;
}
* A_OBJECT ;
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
char * Salloc ( ) ;
SHORT * s_alloc ( ) ;
int * i_alloc ( ) ;
char * Srealloc ( ) ;
char * Scopy ( ) ;
int Ssize ( char * ) ;
void Sfree ( ) ;
void Sarena ( ) ;
void copymem ( ) ;
char * strcpy ( ) ;
T_OBJECT T_create ( ) ;
void T_destroy ( ) ;
int T_member ( ) ;
T_OBJECT T_grow ( ) ;
int T_insert ( ) ;
char * T_name ( ) ;
void T_stats ( ) ;
SHORT * veccpy ( SHORT *, SHORT * ) ;
int veccmp ( SHORT *, SHORT * ) ;
int veclen ( SHORT * ) ;
V_OBJECT V_create ( ) ;
void V_destroy ( ) ;
int V_member ( ) ;
V_OBJECT V_grow ( ) ;
int V_insert ( ) ;
SHORT * V_vec ( ) ;
void V_stats ( ) ;
R_OBJECT R_create ( ) ;
void R_destroy ( ) ;
int R_member ( ) ;
R_OBJECT R_grow ( ) ;
int R_insert ( ) ;
R_row * R_rec ( ) ;
void R_stats ( ) ;
U_OBJECT U_create ( ) ;
void U_destroy ( ) ;
int U_member ( ) ;
U_OBJECT U_grow ( ) ;
int U_insert ( ) ;
A_row * U_rec ( ) ;
void U_stats ( ) ;
A_OBJECT A_create ( ) ;
void A_destroy ( ) ;
A_OBJECT A_rept ( ) ;
void A_exchange ( ) ;
A_OBJECT A_copy ( ) ;
A_OBJECT A_deems ( ) ;
A_OBJECT A_adems ( ) ;
A_OBJECT A_add ( ) ;
A_OBJECT A_open ( ) ;
A_OBJECT A_close ( ) ;
A_OBJECT A_rename ( ) ;
A_OBJECT A_mkdense ( ) ;
A_OBJECT A_load ( ) ;
A_OBJECT A_lwds ( ) ;
A_OBJECT A_store ( ) ;
A_OBJECT A_save ( ) ;
A_OBJECT A_trim ( ) ;
A_OBJECT A_lameq ( ) ;
A_OBJECT A_lamcm ( ) ;
A_OBJECT A_clsure ( ) ;
A_OBJECT A_subs ( ) ;
A_OBJECT A_min ( ) ;
void A_conform ( ) ;
A_OBJECT A_phi ( ) ;
A_OBJECT A_lambda ( ) ;
A_OBJECT A_letter ( ) ;
A_OBJECT A_deecho ( ) ;
A_OBJECT A_opt ( ) ;
A_OBJECT A_plus ( ) ;
A_OBJECT A_star ( ) ;
A_OBJECT A_union ( ) ;
A_OBJECT A_percent ( ) ;
A_OBJECT A_concat ( ) ;
A_OBJECT A_intersect ( ) ;
A_OBJECT A_differ ( ) ;
A_OBJECT A_xor ( ) ;
A_OBJECT A_alph ( ) ;
A_OBJECT A_rev ( ) ;
A_OBJECT A_shuffle ( ) ;
A_OBJECT A_enum ( ) ;
int A_card ( ) ;
A_OBJECT A_pref ( ) ;
A_OBJECT A_suff ( ) ;
A_OBJECT A_compose ( ) ;
A_OBJECT A_join ( ) ;
A_OBJECT A_retape ( ) ;
A_OBJECT A_comma ( ) ;
A_OBJECT A_catpow ( ) ;
A_OBJECT A_ident ( ) ;
A_OBJECT A_cmpow ( ) ;
A_OBJECT A_lenmin ( ) ;
int A_minlen ( A_OBJECT ) ;
A_OBJECT A_sseq ( ) ;
A_OBJECT A_LMsseq ( ) ;
A_OBJECT A_GMsseq ( ) ;
A_OBJECT A_prsseq ( ) ;
A_OBJECT A_clsseq ( ) ;
void A_st_free ( ) ;
SHORT ** A_stems ( ) ;
void A_prstems ( A_OBJECT, T_OBJECT, int ) ;
extern int A_report ;
void Error ( char * ) ;
void Warning ( char * ) ;
void pr_time_diff ( ) ;
int tonum ( char * ) ;
