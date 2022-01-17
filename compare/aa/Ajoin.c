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
A_OBJECT A_join ( A_OBJECT A1, A_OBJECT A2 )
{
  A_OBJECT A ;
  int current, s1, s2, t1, t2, cur_a, cur_b, flag ;
  t1 = 0 ;
  t2 = 0 ;
  /* Initialiaze to suppress warning JHJ */
  A_row * p1, * p1z, * p2, * p2z ;
  U_OBJECT U ;
  A_row * cur_st ;

  if ( ! ( A1 -> A_ems && A2 -> A_ems ) ) {
    if ( A1 -> A_ems ) {
      A1 = A_deems ( A1 ) ;
    }

    if ( A2 -> A_ems ) {
      A2 = A_deems ( A2 ) ;
    }
  }

  if ( A1 -> A_nT == 1 && A2 -> A_nT == 1 ) {
    return ( A_intersect ( A1, A2 ) ) ;
  }

  A1 = A_min ( A1 ) ;
  A2 = A_min ( A2 ) ;
  A = A_create ( ) ;
  A -> A_nT = A1 -> A_nT + A2 -> A_nT - 1 ;
  U = U_create ( ) ;

  if ( U_insert ( U, START, START, 1 ) != START ) {
    Error ( "A_join: BOTCH 1" ) ;
  }

  if ( U_insert ( U, FINAL, FINAL, 1 ) != FINAL ) {
    Error ( "A_join: BOTCH 2" ) ;
  }

  for ( current = 0 ;
        current < U -> U_n ;
        current ++ ) {
    cur_st = U_rec ( U, current ) ;
    cur_a = cur_st -> A_a ;
    cur_b = cur_st -> A_b ;
    flag = cur_st -> A_c ;
    p1 = A1 -> A_p [ cur_a ] ;
    p1z = A1 -> A_p [ cur_a + 1 ] ;
    p2 = A2 -> A_p [ cur_b ] ;
    p2z = A2 -> A_p [ cur_b + 1 ] ;
    s1 = s2 = ( - 1 ) ;

    while ( p1 < p1z || p2 < p2z ) {
      if ( s1 < 0 ) {
        if ( p1 < p1z ) {
          if ( A1 -> A_nT == 1 ) {
            s1 = p1 -> A_b ;
            t1 = 0 ;

          } else if ( A1 -> A_nT == 2 ) {
            s1 = p1 -> A_b ;
            t1 = s1 & 1 ;
            s1 >>= 1 ;

          } else {
            s1 = p1 -> A_b / A1 -> A_nT ;
            t1 = p1 -> A_b - s1 * A1 -> A_nT ;
          }

        } else {
          s1 = MAXSHORT ;
        }
      }

      if ( s2 < 0 ) {
        if ( p2 < p2z ) {
          if ( A2 -> A_nT == 1 ) {
            s2 = p2 -> A_b ;
            t2 = 0 ;

          } else if ( A2 -> A_nT == 2 ) {
            s2 = p2 -> A_b ;
            t2 = s2 & 1 ;
            s2 >>= 1 ;

          } else {
            s2 = p2 -> A_b / A2 -> A_nT ;
            t2 = p2 -> A_b - s2 * A2 -> A_nT ;
          }

        } else {
          s2 = MAXSHORT ;
        }
      }

      if ( p1 -> A_b == 1 || p2 -> A_b == 1 ) {
        if ( p1 -> A_b == 1 && p2 -> A_b == 1 ) {
          A = A_add ( A, current, 1, FINAL ) ;
        }

        if ( p1 -> A_b == 1 ) {
          ++ p1 ;
          s1 = ( - 1 ) ;
        }

        if ( p2 -> A_b == 1 ) {
          ++ p2 ;
          s2 = ( - 1 ) ;
        }

      } else if ( s1 <= s2 && t1 < A1 -> A_nT - 1 ) {
        if ( s1 > 0 ) {
          A = A_add ( A, current, ( A -> A_nT == 1 ? s1 : ( A -> A_nT == 2 ? ( s1 << 1 ) + t1 : s1 * A -> A_nT + t1 ) ), U_insert ( U, ( int ) p1 -> A_c, cur_b, 0 ) ) ;
        }

        ++ p1 ;
        s1 = ( - 1 ) ;

      } else if ( s2 < s1 && t2 != 0 ) {
        if ( flag && s2 > 0 ) {
          A = A_add ( A, current, ( A -> A_nT == 1 ? s2 : ( A -> A_nT == 2 ? ( s2 << 1 ) : s2 * A -> A_nT ) ) + t2 + A1 -> A_nT - 1, U_insert ( U, cur_a, ( int ) p2 -> A_c, 1 ) ) ;
        }

        ++ p2 ;
        s2 = ( - 1 ) ;

      } else if ( s1 == s2 && t1 == A1 -> A_nT - 1 && t2 == 0 ) {
        A = A_add ( A, current, ( A -> A_nT == 1 ? s1 : ( A -> A_nT == 2 ? ( s1 << 1 ) + t1 : s1 * A -> A_nT + t1 ) ), U_insert ( U, ( int ) p1 -> A_c, ( int ) p2 -> A_c, 1 ) ) ;
        ++ p1 ;
        ++ p2 ;
        s1 = s2 = ( - 1 ) ;

      } else if ( s1 <= s2 ) {
        ++ p1 ;
        s1 = ( - 1 ) ;

      } else {
        ++ p2 ;
        s2 = ( - 1 ) ;
      }
    }
  }

  A_destroy ( A1 ) ;
  A_destroy ( A2 ) ;
  U_destroy ( U ) ;
  return ( A ) ;
}
