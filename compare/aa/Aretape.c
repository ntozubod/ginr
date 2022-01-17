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
#include "O.h"
A_OBJECT A_retape ( A_OBJECT A1, A_OBJECT A2, T_OBJECT T )
{
  int i ;
  A_row * p ;
  int sigma, tape, ntapes ;
  int old, base, i1, i2 ;
  char * chp ;
  int trans [ 100 ], tp [ 100 ] ;

  if ( A1 -> A_ems ) {
    A1 = A_deems ( A1 ) ;
  }

  if ( A1 == NULL || A2 == NULL ) {
    Error ( "A_rename: NULL Automaton" ) ;
  }

  if ( A1 -> A_nT > 10 || A2 -> A_nT > 10 ) {
    Error ( "A_rename: Too many tapes" ) ;
  }

  A2 = A_min ( A2 ) ;
  A2 -> A_t [ 0 ] . A_a += 1 ;
  A2 -> A_t [ A2 -> A_nrows - 1 ] . A_c += A2 -> A_nrows ;

  for ( i = 0 ;
        i < A2 -> A_nrows ;
        i ++ ) if ( A2 -> A_t [ i ] . A_a != i + 1 || A2 -> A_t [ i ] . A_c != i + 2 ) {
      Error ( "A_retape: improper form for second argument" ) ;
    }

  ntapes = A2 -> A_nT ;
  tp [ 0 ] = 0 ;

  for ( i = 0 ;
        i < A1 -> A_nT ;
        i ++ ) {
    tp [ i + 1 ] = tp [ i ] ;

    for ( p = A2 -> A_t ;
          p < A2 -> A_t + A2 -> A_nrows - 1 ;
          p ++ ) {
      sigma = p -> A_b / A2 -> A_nT ;
      tape = p -> A_b - sigma * A2 -> A_nT ;
      chp = T_name ( T, sigma ) ;
      sigma = chp [ 0 ] - '0' ;

      if ( chp [ 1 ] || sigma < 0 || sigma > 9 ) {
        Error ( "A_retape: Second argument" ) ;
      }

      if ( sigma != i ) {
        continue ;
      }

      trans [ tp [ i + 1 ] ] = tape ;
      ++ tp [ i + 1 ] ;
    }
  }

  A_destroy ( A2 ) ;
  A1 = A_open ( A1 ) ;

  for ( i = A1 -> A_nrows ;
        -- i >= 0 ;
      ) {
    p = A1 -> A_t + i ;

    if ( p -> A_b <= 1 ) {
      continue ;
    }

    sigma = p -> A_b / A1 -> A_nT ;
    tape = p -> A_b - sigma * A1 -> A_nT ;

    if ( sigma == 1 || tp [ tape ] == tp [ tape + 1 ] ) {
      p -> A_b = 0 ;

    } else {
      i1 = tp [ tape ] ;
      i2 = tp [ tape + 1 ] - 1 ;
      p -> A_b = sigma * ntapes + trans [ i1 ] ;

      if ( i1 < i2 ) {
        old = p -> A_c ;
        base = A1 -> A_nQ - i1 ;
        p -> A_c = base + i1 ;

        while ( ++ i1 < i2 ) {
          A1 = A_add ( A1, base + i1 - 1, sigma * ntapes + trans [ i1 ], base + i1 ) ;
        }

        A1 = A_add ( A1, base + i1 - 1, sigma * ntapes + trans [ i1 ], old ) ;
      }
    }
  }

  A1 -> A_nT = ntapes ;
  A1 = A_close ( A1 ) ;
  return ( A1 ) ;
}
A_OBJECT A_comma ( A_OBJECT A1, A_OBJECT A2 )
{
  A_row * p ;
  int sigma, tape, ntapes ;

  if ( A1 -> A_ems ) {
    A1 = A_deems ( A1 ) ;
  }

  if ( A2 -> A_ems ) {
    A2 = A_deems ( A2 ) ;
  }

  if ( A1 == NULL || A2 == NULL ) {
    Error ( "A_comma: NULL Automaton" ) ;
  }

  if ( ( ntapes = A1 -> A_nT + A2 -> A_nT ) > 10 ) {
    Error ( "A_comma: Too many tapes" ) ;
  }

  A2 = A_open ( A2 ) ;

  for ( p = A2 -> A_t + A2 -> A_nrows ;
        -- p >= A2 -> A_t ;
      ) {
    if ( p -> A_b <= 1 ) {
      continue ;
    }

    sigma = p -> A_b / A2 -> A_nT ;
    tape = p -> A_b - sigma * A2 -> A_nT + A1 -> A_nT ;
    p -> A_b = sigma * ntapes + tape ;
  }

  A2 -> A_nT = ntapes ;
  return ( A_concat ( A1, A2 ) ) ;
}
