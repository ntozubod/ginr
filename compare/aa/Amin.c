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
extern FILE * fpout ;
#include "O.h"
#define UNMARK MAXSHORT
#define LAST   (MAXSHORT-1)
A_OBJECT A_min ( A_OBJECT A )
{
  A_row * p ;
  int f, s, j, ns ;
  int x, b ;
  A_row * lo, * hi ;
  int mode ;
  int nB, wait_H, JL_H, hsize ;
  SHORT * in_B, * B_card, * B_H, * B_N, * B_L ;
  SHORT * in_wait, * wait_N ;
  SHORT * in_JL, * JL_N ;
  SHORT * int_H, * int_N ;
  A_row ** heap ;

  if ( A == NULL ) {
    Error ( "A_min: No OBJECT" ) ;
  }

  if ( A -> A_mode < DFA ) {
    A = A_subs ( A ) ;
  }

  mode = A -> A_mode ;

  if ( mode == DFA_MIN || mode == SSEQ_MIN ) {
    return ( A ) ;
  }

  if ( mode == DFA ) {
    mode = DFA_MIN ;
  }

  if ( mode == SSEQ ) {
    mode = SSEQ_MIN ;
  }

  if ( A -> A_nrows == 0 || A -> A_nQ <= 2 ) {
    A -> A_mode = mode ;
    return ( A ) ;
  }

  if ( A_report ) {
    fprintf ( fpout, "--> A_min\n" ) ;
  }

  A = A_open ( A ) ;

  for ( p = A -> A_t + A -> A_nrows ;
        -- p >= A -> A_t ;
      ) {
    s = p -> A_a ;
    p -> A_a = p -> A_c ;
    p -> A_c = s ;
  }

  A = A_close ( A ) ;
  nB = A -> A_nQ ;
  in_B = s_alloc ( A -> A_nQ ) ;
  B_card = s_alloc ( nB ) ;
  B_H = s_alloc ( nB ) ;
  B_N = s_alloc ( A -> A_nQ ) ;
  B_L = s_alloc ( A -> A_nQ ) ;
  in_wait = s_alloc ( nB ) ;
  wait_N = s_alloc ( nB ) ;
  in_JL = s_alloc ( nB ) ;
  JL_N = s_alloc ( nB ) ;
  int_H = s_alloc ( nB ) ;
  int_N = s_alloc ( A -> A_nQ ) ;
  heap = ( A_row ** ) Salloc ( ( A -> A_nQ + 1 ) * sizeof ( A_row * ) ) ;
  nB = 2 ;
  in_B [ START ] = ( in_B [ 2 ] = 0 ) ;
  B_card [ 0 ] = A -> A_nQ - 1 ;
  B_H [ 0 ] = START ;
  B_N [ START ] = 2 ;
  B_L [ 2 ] = START ;
  B_N [ A -> A_nQ - 1 ] = START ;
  B_L [ START ] = A -> A_nQ - 1 ;

  for ( s = A -> A_nQ ;
        -- s >= 3 ;
      ) {
    in_B [ s ] = 0 ;
    B_N [ s - 1 ] = s ;
    B_L [ s ] = s - 1 ;
  }

  in_B [ FINAL ] = 1 ;
  B_card [ 1 ] = 1 ;
  B_H [ 1 ] = ( B_N [ FINAL ] = ( B_L [ FINAL ] = FINAL ) ) ;
  in_wait [ 0 ] = 0 ;
  in_wait [ 1 ] = 0 ;
  wait_H = 1 ;
  wait_N [ 1 ] = 0 ;
  wait_N [ 0 ] = LAST ;
  in_JL [ 0 ] = 0 ;
  in_JL [ 1 ] = 0 ;

  for ( s = A -> A_nQ ;
        -- s >= 0 ;
      ) {
    int_N [ s ] = UNMARK ;
  }

  while ( wait_H < LAST ) {
    b = wait_H ;
    x = in_wait [ b ] ;
    hsize = 0 ;
    p = NULL ;

    for ( s = B_H [ b ] ;
          s != B_H [ b ] || p == NULL ;
          s = B_N [ s ] ) {
      p = ( lo = A -> A_p [ s ] ) ;
      hi = A -> A_p [ s + 1 ] - 1 ;

      if ( lo <= hi && x <= hi -> A_b ) {
        while ( lo < hi ) {
          if ( x <= p -> A_b ) {
            hi = p ;

          } else {
            lo = p + 1 ;
          }

          p = lo + ( hi - lo ) / 2 ;
        }

        heap [ ++ hsize ] = lo ;
      }
    }

    if ( hsize == 0 ) {
      in_wait [ b ] = LAST ;
      wait_H = wait_N [ wait_H ] ;
      continue ;
    }

    for ( j = hsize / 2 ;
          j > 0 ;
          -- j ) {
      lo = heap [ j ] ;

      for ( f = j ;
            ( s = 2 * f ) <= hsize ;
            f = s ) {
        if ( s < hsize && heap [ s ] -> A_b > heap [ s + 1 ] -> A_b ) {
          ++ s ;
        }

        if ( lo -> A_b <= heap [ s ] -> A_b ) {
          break ;
        }

        heap [ f ] = heap [ s ] ;
      }

      heap [ f ] = lo ;
    }

    x = heap [ 1 ] -> A_b ;
    in_wait [ b ] = x + 1 ;
    JL_H = LAST ;

    for ( ;
          ;
        ) {
      if ( x != heap [ 1 ] -> A_b || hsize == 0 ) {
        for ( j = JL_H ;
              j < LAST ;
              j = JL_N [ j ] ) {
          if ( in_JL [ j ] < B_card [ j ] ) {
            B_H [ nB ] = LAST ;
            B_card [ nB ] = 0 ;
            in_wait [ nB ] = LAST ;

            for ( s = int_H [ j ] ;
                  s < LAST ;
                  s = ns ) {
              in_B [ s ] = nB ;
              -- B_card [ j ] ;
              ++ B_card [ nB ] ;
              B_L [ ( B_H [ j ] = ( B_N [ B_L [ s ] ] = B_N [ s ] ) ) ] = B_L [ s ] ;
              B_N [ s ] = B_H [ nB ] ;
              B_H [ nB ] = s ;
              ns = int_N [ s ] ;
              int_N [ s ] = UNMARK ;
            }

            for ( s = B_H [ nB ] ;
                  ( ns = B_N [ s ] ) < LAST ;
                  s = ns ) {
              B_L [ ns ] = s ;
            }

            B_L [ ( B_N [ s ] = B_H [ nB ] ) ] = s ;
            in_JL [ nB ] = 0 ;

            if ( in_wait [ j ] < LAST || B_card [ nB ] <= B_card [ j ] ) {
              in_wait [ nB ] = 0 ;
              wait_N [ nB ] = wait_H ;
              wait_H = nB ;
            }

            if ( B_card [ nB ] > B_card [ j ] ) {
              if ( in_wait [ j ] < LAST ) {
                in_wait [ nB ] = in_wait [ j ] ;
                in_wait [ j ] = 0 ;

                if ( j == b ) {
                  hsize = 0 ;
                }

              } else {
                in_wait [ j ] = 0 ;
                wait_N [ j ] = wait_H ;
                wait_H = j ;
              }
            }

            ++ nB ;

          } else {
            for ( s = int_H [ j ] ;
                  s < LAST ;
                  s = ns ) {
              ns = int_N [ s ] ;
              int_N [ s ] = UNMARK ;
            }
          }

          in_JL [ j ] = 0 ;
        }

        if ( hsize == 0 ) {
          break ;
        }

        x = heap [ 1 ] -> A_b ;
        in_wait [ b ] = x + 1 ;
        JL_H = LAST ;
      }

      if ( in_B [ heap [ 1 ] -> A_a ] == b ) {
        s = heap [ 1 ] -> A_c ;

        if ( int_N [ s ] == UNMARK ) {
          j = in_B [ s ] ;

          if ( in_JL [ j ] == 0 ) {
            JL_N [ j ] = JL_H ;
            JL_H = j ;
            int_H [ j ] = LAST ;
          }

          ++ in_JL [ j ] ;
          int_N [ s ] = int_H [ j ] ;
          int_H [ j ] = s ;
        }

        if ( heap [ 1 ] + 1 < A -> A_p [ heap [ 1 ] -> A_a + 1 ] ) {
          lo = heap [ 1 ] + 1 ;

        } else {
          lo = heap [ hsize -- ] ;
        }

      } else {
        lo = heap [ hsize -- ] ;
      }

      for ( f = 1 ;
            ( s = 2 * f ) <= hsize ;
            f = s ) {
        if ( s < hsize && heap [ s ] -> A_b > heap [ s + 1 ] -> A_b ) {
          ++ s ;
        }

        if ( lo -> A_b <= heap [ s ] -> A_b ) {
          break ;
        }

        heap [ f ] = heap [ s ] ;
      }

      heap [ f ] = lo ;
    }
  }

  Sfree ( ( char * ) B_card ) ;
  Sfree ( ( char * ) B_H ) ;
  Sfree ( ( char * ) B_N ) ;
  Sfree ( ( char * ) B_L ) ;
  Sfree ( ( char * ) in_wait ) ;
  Sfree ( ( char * ) wait_N ) ;
  Sfree ( ( char * ) in_JL ) ;
  Sfree ( ( char * ) JL_N ) ;
  Sfree ( ( char * ) int_H ) ;
  Sfree ( ( char * ) int_N ) ;
  Sfree ( ( char * ) heap ) ;
  A = A_open ( A ) ;

  for ( p = A -> A_t + A -> A_nrows ;
        -- p >= A -> A_t ;
      ) {
    s = p -> A_a ;
    p -> A_a = p -> A_c ;
    p -> A_c = s ;
  }

  A = A_close ( A ) ;
  A = A_rename ( A, in_B ) ;
  Sfree ( ( char * ) in_B ) ;
  A -> A_mode = mode ;

  if ( A_report ) {
    fprintf ( fpout, "<-- A_min  " ) ;
    ( void ) A_rept ( A ) ;
  }

  return ( A ) ;
}
