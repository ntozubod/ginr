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

#define UNMARK MAXSHORT
#define LAST   (MAXSHORT-1)

A_OBJECT A_subs( register A_OBJECT A )
{
    register int i, j, tmp;
    register A_row *p, *pz;
    int n, hsize, base, head, current, father, son, gap, vlen;
    A_row *insert, *last, **heap, **pnlam;
    SHORT *set, *vec, *fvec;
    A_OBJECT An;
    V_OBJECT V;

    if ( A == NULL ) {
        Error( "A_subs: No OBJECT" );
    }

    if ( A-> A_nQ >= LAST ) {
        Error( "A_subs: Too many states" );
    }

    if ( A-> A_mode <  NFA_CLOSED ) {
        A = A_clsure( A );
    }

    if ( A-> A_mode >= DFA ) {
        return( A );
    }

    if ( A-> A_nrows == 0 ) {
        A-> A_mode = DFA;
        return( A );
    }

    if ( A_report ) {
        fprintf( fpout, "--> A_subs\n" );
    }

    set = s_alloc( A-> A_nQ );
    vec = s_alloc( A-> A_nQ + 1 );
    heap = (A_row **) Salloc( ( A-> A_nQ + 1 ) * sizeof(A_row *) );
    pnlam = (A_row **) Salloc( A-> A_nQ * sizeof(A_row *) );
    An = A_create();
    V = V_create();
    An-> A_nT = A-> A_nT;

    for (   i = A-> A_nQ;
            --i >= 0; ) {
        set[i] = UNMARK;
    }

    head = LAST;
    vlen = 0;

    for (   i = A-> A_nQ;
            --i >= 0; ) {
        p  = A-> A_p[ i ];
        pz = A-> A_p[ i + 1 ];

        while ( p < pz && p-> A_b == 0 ) {
            ++p;
        }

        pnlam[ i ] = p;
    }

    vec[ ( i = 0 ) ] = START;
    pz = pnlam[ START ];

    for (   p = A-> A_t;
            p < pz;
            ++p ) {
        vec[ ++i ] = p-> A_c;
    }

    vec[ i + 1 ] = MAXSHORT;

    if ( V_insert( V, vec ) != START ) {
        Error( "A_subs: BOTCH 1" );
    }

    vec[ 0 ] = FINAL;
    vec[ 1 ] = MAXSHORT;

    if ( V_insert( V, vec ) != FINAL ) {
        Error( "A_subs: BOTCH 2" );
    }

    for (   current = 0;
            current < V-> V_n;
            current++ ) {
        hsize = 0;
        fvec = V_vec( V, current );

        for (   i = 0;
                ( j = fvec[ i ] ) < MAXSHORT;
                i++ ) {

            if ( pnlam[ j ] != A-> A_p[ j + 1 ] ) {

                heap[ ++hsize ] = pnlam[ j ];
            }
        }

        if ( hsize == 0 ) {
            continue;
        }

        for (   base = hsize / 2;
                base > 0;
                --base ) {
            insert = heap[ base ];

            for (   father = base;
                    ( son = 2 * father ) <= hsize;
                    father = son ) {

                if (    son < hsize
                        && heap[ son ]-> A_b > heap[ son + 1 ]-> A_b ) {
                    son++;
                }

                if ( insert-> A_b <= heap[ son ]-> A_b ) {
                    break;
                }

                heap[ father ] = heap[ son ];
            }

            heap[ father ] = insert;
        }

        last = heap[ 1 ];

        for (;;) {

            if ( last-> A_b != heap[ 1 ]-> A_b || hsize == 0 ) {
                n = 0;

                if ( 8 * vlen < A-> A_nQ ) {

                    while  ( head < LAST ) {
                        vec[ n++ ] = head;
                        tmp = head;
                        head = set[ head ];
                        set[ tmp ] = UNMARK;
                    }

                    for (   gap = n / 2;
                            gap > 0;
                            gap /= 2 ) {

                        for (   i = gap;
                                i < n;
                                i++ ) {

                            for (   j = i - gap;
                                    j >= 0 && vec[ j ] > vec[ j + gap ];
                                    j -= gap ) {
                                tmp = vec[ j ];
                                vec[ j ] = vec[ j + gap ];
                                vec[ j + gap ] = tmp;
                            }
                        }
                    }
                }

                else {

                    for (   i = 0;
                            i < A-> A_nQ;
                            ++i ) {

                        if ( set[ i ] != UNMARK ) {
                            set[ ( vec[ n++ ] = i ) ] = UNMARK;
                        }
                    }
                }

                head = LAST;
                vlen = 0;
                vec[ n ] = MAXSHORT;
                An = A_add( An, current, (int) last-> A_b, V_insert( V, vec ) );

                if ( hsize == 0 ) {
                    break;
                }
            }

            i = heap[ 1 ]-> A_c;

            if ( set[ i ] == UNMARK ) {
                set[ i ] = head;
                head = i;
                ++vlen;
                pz = pnlam[ i ];

                for (   p = A-> A_p[ i ];
                        p < pz;
                        ++p ) {
                    j = p-> A_c;

                    if ( set[ j ] == UNMARK ) {
                        set[ j ] = head;
                        head = j;
                        ++vlen;
                    }
                }
            }

            if ( heap[ 1 ] + 1 < A-> A_p[ heap[ 1 ]-> A_a + 1 ] ) {
                insert = heap[ 1 ] + 1;
            }

            else {
                insert = heap[ hsize-- ];
            }

            last = heap[ 1 ];

            for (   father = 1;
                    ( son = 2 * father ) <= hsize;
                    father = son ) {

                if (    son < hsize
                        && heap[ son ]-> A_b > heap[ son + 1 ]-> A_b ) {
                    son++;
                }

                if ( insert-> A_b <= heap[ son ]-> A_b ) {
                    break;
                }

                heap[ father ] = heap[ son ];
            }

            heap[ father ] = insert;
        }
    }

    Sfree( (char *) set );
    Sfree( (char *) vec );
    Sfree( (char *) heap );
    Sfree( (char *) pnlam );
    A_exchange( A, An );
    A_destroy( An );
    V_destroy( V );
    A = A_close( A );
    A-> A_mode = DFA;

    if ( A_report ) {
        fprintf( fpout, "<-- A_subs  " );
        (void) A_rept( A );
    }

    return( A );
}
