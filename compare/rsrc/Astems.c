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

#include <stdio.h>

extern FILE *fpout;

#include "O.h"

extern SHORT *s_rena;
extern int f_rena;

#define WILD (MAXSHORT-1)

static A_OBJECT GAs;
static SHORT  * st_len;
static SHORT  * st_closed;
static SHORT ** st_work = 0;
static SHORT ** st_ptr = 0;
static int st_c_work;

void A_st_free()
{
  int i;

  if ( st_work == 0 ) {
    return;
  }

  if ( st_ptr != st_work ) {
    Sfree( ( char * ) st_ptr );
  }

  for( i = 0; i < st_c_work; i++ ) {
    Sfree( ( char * ) st_work[i] );
  }

  Sfree( ( char * ) st_work );
  st_ptr = st_work = NULL;
}

int A_st_DFS( q, i )
int q, i;
{
  A_row *p;
  int   try, newtry;

  if ( i < st_len[q] ) {
    return( st_work[q][i] );

  } else if ( st_closed[q] ) {
    return( MAXSHORT );
  }

  if ( i > 0 && A_st_DFS( q, i - 1 ) == MAXSHORT ) {
    return( MAXSHORT );
  }

  try = WILD;

  for( p = GAs-> A_p[q]; p < GAs-> A_p[q + 1]; ++p ) {
    if ( p-> A_b == 0 ) {
      newtry = A_st_DFS( p-> A_c, i );

    } else if ( i > 0 ) {
      newtry = A_st_DFS( p-> A_c, i - 1 );

    } else {
      newtry = p-> A_b;
    }

    if ( try == WILD || try == newtry ) try = newtry;

    else {
      try = MAXSHORT;

      st_closed[q] = 1;
      break;
    }
  }

  if ( try == WILD ) try = MAXSHORT;

  st_len[q]++;
  st_work[q] = ( SHORT * ) Srealloc( ( char * ) st_work[q],
                                     st_len[q] * sizeof( SHORT ) );

  if ( i != st_len[q] - 1 ) {
    Error( "A_stems: Bounds check" );
  }

  st_work[q][i] = try;

  return( try );
}

SHORT ** A_stems( A, tape )
A_OBJECT A;
int tape;
{
  int q, i;
  A_OBJECT Aw;
  A_row *p;

  A_st_free();

  if ( A == NULL ) {
    Error( "A_stems: No OBJECT" );
  }

  if ( tape < 0 || tape > A-> A_nT ) {
    Error( "A_stems: tape out of range" );
  }

  Aw = A_copy( A );

  if ( Aw-> A_nT > 1 ) {
    Aw = A_open( Aw );

    for( p = Aw-> A_t + Aw-> A_nrows; --p >= Aw-> A_t; )
      if ( p-> A_b > 1 && p-> A_b % Aw-> A_nT != tape ) {
        p-> A_b = 0;

      } else if ( p-> A_b > 1 ) {
        p-> A_b = p-> A_b / Aw-> A_nT;
      }

    /* else if ( p-> A_b > 1 ) p-> A_b /= Aw-> A_nT; generates bad code? */
    Aw-> A_nT = 1;
  }

  Sfree( ( char * ) s_rena );
  s_rena = 0;
  f_rena = 1;

  if ( Aw-> A_mode < NFA_EQLAM ) {
    Aw = A_lameq( Aw );
  }

  f_rena = 0;
  st_work = ( SHORT ** ) Salloc( Aw-> A_nQ * sizeof( SHORT * ) );
  st_c_work = Aw-> A_nQ;
  st_len  = s_alloc( Aw-> A_nQ );
  st_closed  = s_alloc( Aw-> A_nQ );

  for( i = Aw-> A_nQ; --i >= 0; ) {
    st_work[i] = s_alloc( 1 );
    st_work[i][0] = MAXSHORT;
    st_len[i]  = 0;
    st_closed[i] = 0;
  }

  GAs = Aw;

  for( q = Aw-> A_nQ; --q >= 0; )
    for( i = 0; A_st_DFS( q, i ) != MAXSHORT; i++ );

  Sfree( ( char * ) st_len );
  Sfree( ( char * ) st_closed );
  A_destroy( Aw );

  if ( s_rena == 0 ) {
    st_ptr = st_work;

  } else {
    st_ptr = ( SHORT ** ) Salloc( A-> A_nQ * sizeof( SHORT * ) );

    for( i = 0; i < A-> A_nQ; i++ ) {
      if ( s_rena[i] < MAXSHORT ) {
        st_ptr[i] = st_work[s_rena[i]];

      } else    {
        st_ptr[i] = st_work[1];
      }
    }
  }

  Sfree( ( char * ) s_rena );
  s_rena = 0;

  return( st_ptr );
}

void A_prstems( A, T, tape )
A_OBJECT A;
T_OBJECT T;
int tape;
{
  int   q, i, ch;
  char *        s;

  if ( A == NULL ) {
    Error( "A_prstems: No OBJECT" );
  }

  ( void ) A_stems( A, tape );

  for( q = 0; q < A-> A_nQ; q++ ) {
    if ( q == 0 ) {
      printf( "  (START)  " );

    } else if ( q == 1 ) {
      continue;

    } else {
      printf( "%9d  ", q );
    }

    for( i = 0; st_ptr[q][i] < MAXSHORT; i++ ) {
      ch = st_ptr[q][i];
      s = T_name( T, ch );

      if ( s[0] == '\n' ) {
        s = "\\n";

      } else if ( s[0] == '\t' ) {
        s = "\\t";

      } else if ( s[0] == ' '  ) {
        s = "\\_";
      }

      printf( " %s", s );
    }

    printf( "\n" );
  }

  A_st_free();
}
