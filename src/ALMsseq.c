/*
 * Copyright (c) 1987, J Howard Johnson, University of Waterloo.
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

extern FILE *fpout;
extern T_OBJECT TT;

#define UNMARK          MAXSHORT
#define LAST            (MAXSHORT-1)

A_OBJECT A_LMsseq( A )
register A_OBJECT A;
{
    register int i, j, tmp;
    register A_row *p, *pz;
    int tt;
    int n, hsize, base, head, current, father, son, gap, vlen;
    int k, sig_lim;
    int aa, bb, cc, nq, len, from, to, label, hi_next, try;
    A_row *insert, *last, **heap;
    SHORT *set, *vec, *fvec, **fr_coeff, **to_coeff, *queue, *st_len;
    SHORT ** st_ptr, *save_coeff, *sig, *back;
    A_OBJECT An;
    V_OBJECT V, Vs, Vsig;

    if ( A == NULL ) Error( "A_sseq: No OBJECT" );
    if ( A-> A_nQ >= LAST ) Error( "A_sseq: Too many states" );
    A = A_close( A );
    if ( A-> A_mode < DFA_MIN ) A = A_min( A );
    if ( A-> A_nrows == 0 ) {
        A-> A_mode = DFA;
        return( A );
    }
    if ( A-> A_nT != 2 ) Error( "A_sseq: Not transducer" );
    tmp = 0;
    for( i = A-> A_nrows; --i >= 0; )
        if ( A-> A_t[i].A_b / 2 == 1 ) {
            tmp = 1;
            A-> A_t[i].A_b = 0;
        }
    if ( tmp ) A = A_min( A_open( A ) );

    if ( A_report ) fprintf( fpout, "--> A_sseq\n" );
    set = s_alloc( A-> A_nQ );
    vec = s_alloc( 2 * A-> A_nQ + 1 );
    heap = (A_row **) Salloc( (A-> A_nQ + 1) * sizeof(A_row *) );
    fr_coeff = (SHORT **) Salloc( A-> A_nQ * sizeof(SHORT *) );
    to_coeff = (SHORT **) Salloc( A-> A_nQ * sizeof(SHORT *) );
    queue = s_alloc( A-> A_nQ );
    st_len = s_alloc( A-> A_nQ );
    sig_lim = 20;
    sig = s_alloc( sig_lim );
    back = s_alloc( sig_lim );
    An = A_create();
    V = V_create();
    Vs = V_create();
    Vsig = V_create();
    An-> A_nT = A-> A_nT;
    for( i = A-> A_nQ; --i >= 0; ) set[i] = UNMARK;
    head = LAST;
    vlen = 0;
    st_ptr = (SHORT **) A_stems( A, 1 );
    for( k = 0; k < A-> A_nQ; k++ ) st_len[k] = veclen( st_ptr[k] );
    hi_next = MAXSHORT - 2;

    /* Construct start state as the write closure of START.
     */

    set[ START ] = head;
    head = START;
    ++vlen;
    len = veclen( st_ptr[ START ] );
    to_coeff[START] = s_alloc( len + 1 );
    veccpy( to_coeff[ START ], st_ptr[ START ] );
    queue[ 0 ] = START;
    nq = 1;
    for( i = 0; i < nq; i++ ) {
        pz = A-> A_p[ queue[i] + 1 ];
        for( p = A-> A_p[ queue[i] ]; p < pz; ++p ) {
            if ( p-> A_b != 1 && p-> A_b % 2 == 1 ) {
                if ( p-> A_a == p-> A_c )
                    continue;
                /* Error( "A_sseq: Relation not single-valued (S2)" ); */
                if ( set[ p-> A_c ] == UNMARK ) {
                    set[ p-> A_c ] = head;
                    head = p-> A_c;
                    ++vlen;
                    save_coeff = 0;
                    queue[ nq++ ] = p-> A_c;
                } else {
                    save_coeff = to_coeff[ p-> A_c ];
                }
                len = veclen( to_coeff[ p-> A_a ] ) + 1
                      - st_len[ p-> A_a ] + st_len[ p-> A_c ];
                to_coeff[ p-> A_c ] = s_alloc( len + 1 );
                veccpy( to_coeff[ p-> A_c ], to_coeff[ p-> A_a ] );
                if ( st_len[ p-> A_a ] == 0 ) {
                    to_coeff[ p-> A_c ]
                    [ len - st_len[ p-> A_c ] - 1 ] = p-> A_b / 2;
                    veccpy( to_coeff[ p-> A_c ] + ( len - st_len[ p-> A_c ] ),
                            st_ptr[ p-> A_c ] );
                }
                if ( save_coeff ) {
                    if ( veccmp( save_coeff, to_coeff[ p-> A_c ] )
                            && veclen(save_coeff) == veclen(to_coeff[p->A_c]) )
                        Error( "A_sseq: Relation not single-valued (S3)" );
                    if ( veclen( save_coeff ) >= veclen( to_coeff[p->A_c] ) )
                        Sfree( (char *) save_coeff );
                    else {
                        Sfree( (char *) to_coeff[p->A_c] );
                        to_coeff[p->A_c] = save_coeff;
                    }
                }
            }
        }
    }
    n = 0;
    head = LAST;
    for( i = 0; i < A-> A_nQ; ++i )
        if ( set[ i ] != UNMARK ) {
            set[ i ] = UNMARK;
            pz = A-> A_p[i+1];
            for( p = A-> A_p[i]; p < pz; p++ )
                if ( p-> A_b == 1 || p-> A_b % 2 == 0 ) break;
            if ( i == FINAL || p < pz )
                vec[n++] = i;
            else    Sfree( (char *) to_coeff[i] );
        }
    vlen = n;

    for( i = 0; i < vlen; i++ )
        vec[i+vlen] = V_insert( Vs, to_coeff[vec[i]] );
    vec[ vlen + vlen ] = MAXSHORT;
    if ( V_insert( V, vec ) != START ) Error( "A_sseq: BOTCH 1" );
    current = START;

    if ( V_insert( V, vec + vlen + vlen ) != FINAL ) Error( "A_sseq: BOTCH 2" );

    to = START;
    for( j = 0; ; j++ ) {
        try = to_coeff[vec[0]][j];
        if ( try == MAXSHORT ) goto idone;
        for( i = 1; i < vlen; i++ )
            if ( to_coeff[vec[i]][j] != try ) goto idone;
        if ( to != START ) An = A_add( An, from, label, to );
        from = to;
        label = 2 * try + 1;
        to = hi_next--;
    }
idone:
    if ( j > 0 ) {
        ++hi_next;
        for( i = 0; i < vlen; i++ )
            vec[i+vlen] = V_insert( Vs, to_coeff[vec[i]] + j );
        vec[ vlen + vlen ] = MAXSHORT;
        An = A_add( An, from, label, current = V_insert( V, vec ) );
    }
    for( i = 0; i < vlen; i++ ) Sfree( (char *) to_coeff[vec[i]] );
    vlen = 0;

    vec[ 0 ] = FINAL;
    vec[ 2 ] = MAXSHORT;
    vec[ 1 ] = V_insert( Vs, vec + 2 );
    An = A_add( An, V_insert( V, vec ), 1, FINAL );

    sig[0]  = sig[1]  = sig[2]  = MAXSHORT;
    back[0] = back[1] = back[2] = 0;

    for( ; current < V-> V_n; current++ ) {
        if ( current == FINAL ) continue;

        /*     Unpack current state:
         * (1) fvec is set to vector of component states followed by their coeffs
         * (2) load heap with positions in source automaton for each component state
         * (3) load fr_coeff with the pointers to coeff vectors
         * (4) heapify the heap
         */

        hsize = 0;
        fvec = V_vec( V, current );
        len = veclen( fvec ) / 2;
        for( i = 0; i < len; i++ ) {
            j = fvec[ i ];
            if ( A-> A_p[ j ] != A-> A_p[ j + 1 ] )
                heap[ ++hsize ] = A-> A_p[ j ];
            fr_coeff[ j ] = V_vec( Vs, fvec[ len + i ] );
        }
        /*
        printf( "Processing state %d\n", current );
        printf( "state coeff\n" );
        for( i = 0; i < len; i++ ) {
        j = fvec[ i ];
        printf( "%5d ", j );
        for( tt = 0; fr_coeff[j][tt] != MAXSHORT; tt++ )
        printf( "%s ", T_name( TT, fr_coeff[j][tt] ) );
        printf( "\n" );
        }
        */
        if ( hsize == 0 ) continue;
        for( base = hsize / 2; base > 0; --base ) {
            insert = heap[ base ];
            for( father = base; (son = 2 * father) <= hsize; father = son ) {
                if ( son < hsize && heap[son]-> A_b > heap[son+1]-> A_b ) son++;
                if ( insert-> A_b <= heap[son]-> A_b ) break;
                heap[father] = heap[son];
            }
            heap[father] = insert;
        }

        /* Main loop to process current state: Each cycle processes position at the
         * top of the heap.
         */

        last = heap[1];
        for(;;) {

            /* End processing for an output state.
             */

            if ( ( last-> A_b != heap[1]-> A_b || hsize == 0 ) && vlen > 0 ) {
                n = 0;
                if ( 8 * vlen < A-> A_nQ ) {
                    vlen = 0;
                    while  ( head < LAST ) {
                        pz = A-> A_p[head+1];
                        for( p = A-> A_p[head]; p < pz; p++ )
                            if ( p-> A_b == 1 || p-> A_b % 2 == 0 ) break;
                        if ( head == FINAL || p < pz )
                            vec[n++] = head;
                        else    Sfree( (char *) to_coeff[head] );
                        tmp = head;
                        head = set[ head ];
                        set[ tmp ] = UNMARK;
                    }
                    for( gap = n / 2; gap > 0; gap /= 2 )
                        for( i = gap; i < n; i++ )
                            for( j = i - gap; j >= 0 && vec[j] > vec[j+gap];
                                    j -= gap ) {
                                tmp = vec[j];
                                vec[j] = vec[j+gap];
                                vec[j+gap] = tmp;
                            }
                } else {
                    for( i = 0; i < A-> A_nQ; ++i )
                        if ( set[ i ] != UNMARK ) {
                            set[ i ] = UNMARK;
                            pz = A-> A_p[i+1];
                            for( p = A-> A_p[i]; p < pz; p++ )
                                if ( p-> A_b == 1 || p-> A_b % 2 == 0 ) break;
                            if ( i == FINAL || p < pz )
                                vec[n++] = i;
                            else    Sfree( (char *) to_coeff[i] );
                        }
                }
                vlen = n;
                head = LAST;

                from = current;
                label = last-> A_b;
                if ( label == 1 ) label = 2;
                to = hi_next;
                /*
                printf( "Destination state\n" );
                printf( "state coeff\n" );
                for( i = 0; i < vlen; i++ ) {
                j = vec[ i ];
                printf( "%5d ", j );
                for( tt = 0; to_coeff[j][tt] != MAXSHORT; tt++ )
                printf( "%s ", T_name( TT, to_coeff[j][tt] ) );
                printf( "\n" );
                }
                */
                for( j = 0; ; j++ ) {
                    try = to_coeff[vec[0]][j];
                    if ( try == MAXSHORT ) goto done;
                    for( i = 1; i < vlen; i++ )
                        if ( to_coeff[vec[i]][j] != try ) goto done;
                    An = A_add( An, from, label, to );
                    from = to;
                    label = 2 * try + 1;
                    to = --hi_next;
                    if ( to < V-> V_n ) Error( "A_sseq: Overflow" );
                }
done:
                for( i = 0; i < vlen; i++ ) {
                    vec[i+vlen] = V_insert( Vs, to_coeff[vec[i]] + j );
                    Sfree( (char *) to_coeff[vec[i]] );
                }
                vec[ vlen + vlen ] = MAXSHORT;
                tmp = V-> V_n;
                An = A_add( An, from, label, to = V_insert( V, vec ) );
                if ( to == tmp ) {
                    vec[ vlen ] = MAXSHORT;
                    tmp = Vsig-> V_n;
                    i = V_insert( Vsig, vec );
                    if ( to  >= sig_lim ) {
                        sig_lim *= 2;
                        sig  = (SHORT *) Srealloc( (char *) sig,
                                                   sig_lim * sizeof(SHORT));
                        back = (SHORT *) Srealloc( (char *) back,
                                                   sig_lim * sizeof(SHORT));
                    }
                    sig[ to ] = i;
                    back[ to ] = current;
                    if ( i < tmp ) {
                        j = 1;
                        for( k = current; k > 0; k = back[ k ] )
                            if  ( sig[k] == i ) ++j;
                        if ( j > vlen )
                            Error( "A_sseq: Not subsequential (?)" );
                    }
                }
                vlen = 0;
            }

            if ( hsize == 0 ) break;

            /* Main processing for a transition from the input automaton.
             * Add state to subset and compute coefficient.
             */

            aa = heap[1]-> A_a;
            bb = heap[1]-> A_b;
            cc = heap[1]-> A_c;
            if ( bb == 1 || bb % 2 == 0 ) {
                if ( set[ cc ] == UNMARK ) {
                    set[ cc ] = head;
                    head = cc;
                    ++vlen;
                    save_coeff = 0;
                } else {
                    save_coeff = to_coeff[ cc ];
                }
                len = veclen( fr_coeff[ aa ] ) - st_len[ aa ] + st_len[ cc ];
                if ( bb == 1 ) ++len;
                if ( len < 0 ) Error( "A_sseq: len error" );
                to_coeff[ cc ] = s_alloc( len + 1 );
                veccpy( to_coeff[ cc ], fr_coeff[ aa ] );
                if ( st_len[ aa ] < st_len[ cc ] )
                    veccpy( to_coeff[ cc ] + veclen( to_coeff[ cc ] ),
                            st_ptr[ cc ] + st_len[ aa ] );
                if ( bb == 1 && len > 0 ) {
                    to_coeff[ cc ][ len - 1 ] = 1;
                    to_coeff[ cc ][ len ] = MAXSHORT;
                }
                if ( save_coeff ) {
                    if ( veccmp( save_coeff, to_coeff[ cc ] )
                            && veclen( save_coeff ) == veclen( to_coeff[cc] ) )
                        Error( "A_sseq: Relation not single-valued (1)" );
                    if ( veclen( save_coeff ) >= veclen( to_coeff[cc] ) )
                        Sfree( (char *) save_coeff );
                    else {
                        Sfree( (char *) to_coeff[cc] );
                        to_coeff[cc] = save_coeff;
                    }
                } else {
                    queue[ 0 ] = cc;
                    nq = 1;
                    for( i = 0; i < nq; i++ ) {
                        pz = A-> A_p[ queue[i] + 1 ];
                        for( p = A-> A_p[ queue[i] ]; p < pz; ++p ) {
                            if ( p-> A_b != 1 && p-> A_b % 2 == 1 ) {
                                if ( p-> A_a == p-> A_c )
                                    continue;
                                /* Error( "A_sseq: Relation not single-valued (2)" ); */
                                if ( set[ p-> A_c ] == UNMARK ) {
                                    set[ p-> A_c ] = head;
                                    head = p-> A_c;
                                    ++vlen;
                                    save_coeff = 0;
                                    queue[ nq++ ] = p-> A_c;
                                } else {
                                    save_coeff = to_coeff[ p-> A_c ];
                                }
                                len = veclen( to_coeff[ p-> A_a ] ) + 1
                                      - st_len[ p-> A_a ] + st_len[ p-> A_c ];
                                to_coeff[ p-> A_c ] = s_alloc( len + 1 );
                                veccpy( to_coeff[ p-> A_c ],
                                        to_coeff[ p-> A_a ] );
                                tt = len - st_len[ p-> A_c ] - 1;
                                if ( tt >= 0 && tt < len )
                                    to_coeff[ p-> A_c ][ tt ] = p-> A_b / 2;
                                tt = len - st_len[ p-> A_c ];
                                if ( tt < 0 ) tt = 0;
                                if ( tt <= len )
                                    veccpy( to_coeff[ p-> A_c ] + tt,
                                            st_ptr[ p-> A_c ]
                                            + tt - ( len - st_len[ p-> A_c ] ) );
                                if ( save_coeff ) {
                                    if ( veccmp( save_coeff,
                                                 to_coeff[ p-> A_c ] )
                                            && veclen( save_coeff ) ==
                                            veclen( to_coeff[ p-> A_c ] ) ) {

                                        fprintf( fpout, "Error detected at state %d:\n", p-> A_c );
                                        fprintf( fpout, "coeffs are\n" );
                                        for( k = 0; to_coeff[p->A_c][k] < MAXSHORT; k++ )
                                            fprintf( fpout, "%s ",
                                                     T_name( TT, to_coeff[p->A_c][k] ) );
                                        fprintf( fpout, "\n" );
                                        for( k = 0; save_coeff[k] < MAXSHORT; k++ )
                                            fprintf( fpout, "%s ",
                                                     T_name( TT, save_coeff[k] ) );
                                        fprintf( fpout, "\n" );
                                        Error( "A_sseq: Relation not single-valued (3)" );

                                    }
                                    if ( veclen( save_coeff ) >=
                                            veclen( to_coeff[ p-> A_c ] ) )
                                        Sfree( (char *) save_coeff );
                                    else {
                                        Sfree( (char *) to_coeff[ p-> A_c ] );
                                        to_coeff[p-> A_c] = save_coeff;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if ( heap[1] + 1 < A-> A_p[ heap[1]-> A_a + 1 ] )
                insert = heap[1] + 1;
            else    insert = heap[ hsize-- ];
            last = heap[1];
            for( father = 1; (son = 2 * father) <= hsize; father = son ) {
                if ( son < hsize && heap[son]-> A_b > heap[son+1]-> A_b ) son++;
                if ( insert-> A_b <= heap[son]-> A_b ) break;
                heap[father] = heap[son];
            }
            heap[ father ] = insert;
        }
    }

    Sfree( (char *) set );
    Sfree( (char *) vec );
    Sfree( (char *) sig );
    Sfree( (char *) back );
    Sfree( (char *) heap );
    Sfree( (char *) fr_coeff );
    Sfree( (char *) to_coeff );
    Sfree( (char *) queue );
    Sfree( (char *) st_len );
    A_st_free();
    A_exchange( A, An );
    A_destroy( An );
    V_destroy( V );
    V_destroy( Vs );
    A = A_rename( A, 0 );
    A = A_close( A );
    A-> A_mode = SSEQ;
    A-> A_ems = 1;
    if ( A_report ) {
        fprintf( fpout, "<-- A_sseq  " );
        (void) A_rept( A );
    }
    return( A );
}
