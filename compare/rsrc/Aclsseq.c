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

A_OBJECT A_clsseq( A1 )
register A_OBJECT A1;
{
  register A_OBJECT A;
  int current, end_st, i, bb, last_label, label, hi_next, k;
  SHORT *vec, *cur_vec;
  V_OBJECT V;
  A_row *p, *pz, *lo, *hi, *mid;

  A1 = ( A_OBJECT ) A_sseq( A1 );
  A = A_create();
  A-> A_nT = 2;
  V = V_create();
  vec = s_alloc( 101 );

  vec[0] = START;
  vec[1] = MAXSHORT;

  if ( V_insert( V, vec ) != START ) {
    Error( "A_clsseq: BOTCH 1" );
  }

  vec[0] = FINAL;
  vec[1] = FINAL;
  vec[2] = MAXSHORT;

  if ( V_insert( V, vec ) != FINAL ) {
    Error( "A_clsseq: BOTCH 2" );
  }

  hi_next = MAXSHORT - 1;

  for( current = 0; current < V-> V_n; current++ ) {
    if ( current == FINAL ) {
      continue;
    }

    cur_vec = V_vec( V, current );
    end_st = cur_vec[ veclen( cur_vec ) - 1 ];
    pz = A1-> A_p[ cur_vec[0] + 1 ];

    for( p = A1-> A_p[ cur_vec[0] ]; p < pz; ++p ) {
      if ( p-> A_b == 1 ) {
        /*
                        for( k = 1; vec[k] < MAXSHORT; k++ )
                            if ( A1-> A_p[ vec[k] ] == A1-> A_p[ vec[k] + 1 ]
                              || A1-> A_p[ vec[k] ]-> A_b != 1 ) break;
                        if ( vec[k] == MAXSHORT )
        */
        A = A_add( A, current, 1, FINAL );
        continue;
      }

      veccpy( vec, cur_vec );
      label = p-> A_b / 2;
      i = 0;
      k = 0;

      if ( label != 1 ) {
        k = 2 * label;
      }

      A = A_add( A, current, k, hi_next-- );

      if ( hi_next < V-> V_n ) {
        Error( "A_clsseq: Too many states" );
      }

      while( i >= 0 ) {
        lo = A1-> A_p[ vec[i] ];
        hi = A1-> A_p[ vec[i] + 1 ] - 1;

        if ( lo > hi || lo-> A_b == 1 || lo -> A_b % 2 == 0 ) {
          if ( label ) {
            bb = 2 * label;

            while ( lo < hi ) {
              mid = lo + ( hi - lo ) / 2;

              if ( bb <= mid-> A_b ) {
                hi = mid;

              } else {
                lo = mid + 1;
              }
            }

            if ( lo == hi && lo-> A_b == bb ) {
              vec[i] = lo-> A_c;

            } else {
              vec[i] = FINAL;
            }

            last_label = label;
            label = 0;

          } else {
            if ( i >= 98 ) {
              Error( "A_clsseq: State explosion" );
            }

            if ( vec[i + 1] == MAXSHORT ) {
              vec[i + 1] = end_st;
              vec[i + 2] = MAXSHORT;
            }

            --i;
          }

        } else {
          vec[i] = lo-> A_c;
          ++i;

          if ( i >= 100 ) {
            Error( "A_clsseq: State explosion" );
          }

          label = lo-> A_b / 2;

          if ( vec[i] == MAXSHORT ) {
            if ( i >= 2 && vec[i - 1] == vec[i - 2]
                 && label == last_label ) {
              k = 0;

              if ( label != 1 ) {
                k = 2 * label + 1;
              }

              A = A_add( A, hi_next + 1, k, hi_next );
              --hi_next;

              if ( hi_next < V-> V_n ) {
                Error( "A_clsseq: Too many states" );
              }

              --i;
              label = 0;

            } else {
              vec[i] = end_st;
              vec[i + 1] = MAXSHORT;
            }
          }
        }
      }

      i = veclen( vec );

      while ( i > 1 && vec[i - 1] == vec[i - 2] ) {
        --i;
      }

      vec[ i ] = MAXSHORT;
      i = 0;

      while ( vec[i] == vec[i + 1] ) {
        ++i;
      }

      A = A_add( A, hi_next + 1, 0, ( k = V_insert( V, vec + i ) ) );
    }
  }

  A_exchange( A, A1 );
  A_destroy( A );
  V_destroy( V );
  Sfree( ( char * ) vec );
  /*
      A1 = A_min( A_rename( A1, 0 ) );
  */
  A1 = A_min( A_mkdense( A1 ) );
  A1-> A_mode = SSEQ_MIN;
  A1-> A_ems = 1;
  return( A1 );
}
