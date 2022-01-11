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
#include "O.h"
A_OBJECT A_lenmin ( A ) A_OBJECT A ;
{
  A_row * lo, * hi, * p ;
  int i, l, cur, dead ;
  SHORT * bfs, * dis ;

  if ( A -> A_nrows == 0 ) {
    return ( A ) ;
  }

  A = A_subs ( A ) ;
  bfs = s_alloc ( A -> A_nQ ) ;
  dis = s_alloc ( A -> A_nQ ) ;

  for ( i = 0 ;
        i < A -> A_nQ ;
        i ++ ) {
    dis [ i ] = MAXSHORT ;
  }

  bfs [ 0 ] = START ;
  l = 1 ;
  dis [ START ] = 0 ;

  for ( i = 0 ;
        i < l ;
        i ++ ) {
    cur = bfs [ i ] ;
    lo = A -> A_p [ cur ] ;
    hi = A -> A_p [ cur + 1 ] ;

    for ( p = lo ;
          p < hi ;
          p ++ ) {
      if ( dis [ p -> A_c ] > dis [ cur ] + 1 ) {
        dis [ p -> A_c ] = dis [ cur ] + 1 ;
        bfs [ l ++ ] = p -> A_c ;
      }
    }
  }

  A = A_open ( A ) ;
  lo = A -> A_t ;
  hi = lo + A -> A_nrows ;
  dead = A -> A_nQ ++ ;

  for ( p = lo ;
        p < hi ;
        p ++ ) if ( dis [ p -> A_a ] >= dis [ p -> A_c ] ) {
      p -> A_c = dead ;
    }

  Sfree ( ( char * ) bfs ) ;
  Sfree ( ( char * ) dis ) ;
  return ( A_trim ( A ) ) ;
}
int A_minlen ( A_OBJECT A )
{
  A_row * lo, * hi, * p ;
  int i, l, cur ;
  SHORT * bfs, * dis ;

  if ( A -> A_nrows == 0 ) {
    return ( - 1 ) ;
  }

  A = A_subs ( A ) ;
  bfs = s_alloc ( A -> A_nQ ) ;
  dis = s_alloc ( A -> A_nQ ) ;

  for ( i = 0 ;
        i < A -> A_nQ ;
        i ++ ) {
    dis [ i ] = MAXSHORT ;
  }

  bfs [ 0 ] = START ;
  l = 1 ;
  dis [ START ] = 0 ;

  for ( i = 0 ;
        i < l ;
        i ++ ) {
    cur = bfs [ i ] ;
    lo = A -> A_p [ cur ] ;
    hi = A -> A_p [ cur + 1 ] ;

    for ( p = lo ;
          p < hi ;
          p ++ ) {
      if ( dis [ p -> A_c ] == MAXSHORT ) {
        dis [ p -> A_c ] = dis [ cur ] + 1 ;
        bfs [ l ++ ] = p -> A_c ;
      }
    }
  }

  i = dis [ FINAL ] - 1 ;
  Sfree ( ( char * ) bfs ) ;
  Sfree ( ( char * ) dis ) ;
  return ( i ) ;
}
