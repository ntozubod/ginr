extern int S_flag;
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

SHORT *s_rena = 0;
int f_rena = 0;

A_OBJECT A_add( A, a, b, c )
register A_OBJECT A;
register int a, b, c;
{
    register A_row *p;

    if ( A == NULL ) Error( "A_add: No OBJECT" );
    if ( A-> A_mode != OPEN ) Error( "A_add: Object not OPEN" );
    if ( a < 0 || b < 0 || c < 0 )
        Error( "A_add: Numbers cannot be negative" );
    if ( a >= MAXSHORT || b >= MAXSHORT || c >= MAXSHORT )
        Error( "A_add: Numbers are too big" );
    if ( A-> A_nrows >= A-> A_lrows ) {
        A-> A_t = (A_row *) Srealloc( (char *) A-> A_t,
                                      ( 2 * A-> A_nrows + 2 ) * sizeof(A_row) );
        A-> A_lrows = Ssize( (char *) A-> A_t ) / sizeof(A_row) - 2;
    }
    p = A-> A_t + A-> A_nrows++;
    p-> A_a = a;
    p-> A_b = b;
    p-> A_c = c;
    if ( a >= A-> A_nQ ) A-> A_nQ = a + 1;
    if ( b >= A-> A_nS * A-> A_nT ) A-> A_nS = b / A-> A_nT + 1;
    if ( c >= A-> A_nQ ) A-> A_nQ = c + 1;
    return( A );
}

A_OBJECT A_open( A )
register A_OBJECT A;
{
    if ( A == NULL ) Error( "A_open: No OBJECT" );
    A-> A_mode = OPEN;
    Sfree( (char *) A-> A_p );
    A-> A_p =       NULL;
    return( A );
}

A_OBJECT A_close( A )
register A_OBJECT A;
{
    register int i;
    register A_row *p, *q, *t1, *t2;

    int N, NQ, NS;
    A_row *t1z, *t2z;
    int *cnt;
    A_row **ptr;

    if ( A == NULL ) Error( "A_close: No OBJECT" );
    if ( A-> A_mode != OPEN ) return( A );

    if ( A-> A_nrows == 0 ) {
        A-> A_mode = NFA;
        A-> A_ems = 0;
        A-> A_nT = 1;
        A-> A_nQ = 2;
        A-> A_nS = 2;
        A-> A_p = (A_row **) Salloc( 3 * sizeof(A_row *) );
        A-> A_p[ START ] = A-> A_p[ FINAL ]
                           = A-> A_p[ 2 ] = A-> A_t;
        return( A );
    }

    if ( A_report ) fprintf( fpout, "--> A_close\n" );
    NQ = A-> A_nQ;
    NS = A-> A_nS * A-> A_nT;
    N = ( NQ > NS ) ? NQ : NS;

    t1  = A-> A_t;
    t2  = (A_row *) Salloc( ( A-> A_nrows + 2 ) * sizeof(A_row) );
    t1z = t1 + A-> A_nrows;
    t2z = t2 + A-> A_nrows;

    cnt = (int *) Salloc( N * sizeof(int) );
    ptr = (A_row **) Salloc( ( N + 1 ) * sizeof(A_row *) );
    for( i = N; --i >= 0; ) cnt[i] = 0;

    for( p = t1z; --p >= t1; ) ++cnt[ p-> A_c ];

    p = t2z;
    for( i = NQ; --i >= 0; ) {
        ptr[ i ] = p;
        p -= cnt[ i ];
        cnt[ i ] = 0;
    }
    for( p = t1z; --p >= t1; ) {
        q = --ptr[ i = p-> A_c ];
        q-> A_a = p-> A_a;
        ++cnt[ q-> A_b = p-> A_b ];
        q-> A_c = i;
    }

    p = t1z;
    for( i = NS; --i >= 0; ) {
        ptr[ i ] = p;
        p -= cnt[ i ];
        cnt[ i ] = 0;
    }
    for( p = t2z; --p >= t2; ) {
        q = --ptr[ i = p-> A_b ];
        ++cnt[ q-> A_a = p-> A_a ];
        q-> A_b = i;
        q-> A_c = p-> A_c;
    }

    p = t2z;
    for( i = NQ; --i >= 0; ) {
        ptr[ i ] = p;
        p -= cnt[ i ];
        cnt[ i ] = 0;
    }
    for( p = t1z; --p >= t1; ) {
        q = --ptr[ i = p-> A_a ];
        q-> A_a = i;
        q-> A_b = p-> A_b;
        q-> A_c = p-> A_c;
    }

    Sfree( (char *) A-> A_t );
    A-> A_t = t2;

    t2z-> A_a = (t2z - 1)-> A_a;
    t2z-> A_b = (t2z - 1)-> A_b;
    t2z-> A_c = (t2z - 1)-> A_c;
    (t2z + 1)-> A_a = MAXSHORT;

    p = t2;
    t1 = t2 - 1;
    while ( t2 < t2z ) {
        q = t2;
        ++cnt[ t2-> A_a ];
        while ( (++t1)-> A_c != (++t2)-> A_c
                ||    t1 -> A_b !=    t2 -> A_b
                ||    t1 -> A_a !=    t2 -> A_a ) ++cnt[ t2-> A_a ];
        if ( p < q )
            copymem( ( t2-q ) * sizeof(A_row),
                     (char *) q, (char *) p );
        p += t2 - q;
        while ( (++t1)-> A_c == (++t2)-> A_c
                &&    t1 -> A_b ==    t2 -> A_b
                &&    t1 -> A_a ==    t2 -> A_a );
    }

    A-> A_mode = NFA;
    A-> A_nrows = p - A-> A_t;
    A-> A_lrows = Ssize( (char *) A-> A_t ) / ( sizeof(A_row) ) - 2;

    ptr[ A-> A_nQ ] = p;
    for( i = A-> A_nQ; --i >= 0; ) ptr[ i ] = ( p -= cnt[ i ] );
    A-> A_p = ptr;

    Sfree( (char *) cnt );
    if ( A_report ) fprintf( fpout, "<-- A_close\n" );
    return( A );
}

A_OBJECT A_rename( A, rena )
register A_OBJECT A;
register SHORT *rena;
{
    register A_row  *p, *pz;
    register SHORT  *trena, *sp;
    int             nrena, i;

    if ( A == NULL ) Error( "A_rename: No OBJECT" );

    trena = s_alloc( A-> A_nQ );
    for( sp = trena + A-> A_nQ; --sp >= trena; ) *sp = MAXSHORT;
    pz = A-> A_t + A-> A_nrows;
    if ( rena != NULL ) {
        for( p = A-> A_t; p < pz; ++p ) {
            p-> A_a = rena[ p-> A_a ];
            p-> A_c = rena[ p-> A_c ];
        }
        trena[ rena[ START ] ] = START;
        trena[ rena[ FINAL ] ] = FINAL;
    } else {
        trena[ START ] = START;
        trena[ FINAL ] = FINAL;
    }
    nrena = 2;
    for( p = A-> A_t; p < pz; ++p ) {
        sp = trena + p-> A_a;
        if ( *sp == MAXSHORT ) *sp = nrena++;
        p-> A_a = *sp;
    }
    for( p = pz; --p >= A-> A_t; )
        if ( (p-> A_c = trena[p-> A_c]) == MAXSHORT
                || ( p-> A_a == p-> A_c && p-> A_b == 0 ) ) {
            --pz;
            p-> A_a = pz-> A_a;
            p-> A_b = pz-> A_b;
            p-> A_c = pz-> A_c;
        }

    Sfree( (char *) s_rena );
    s_rena = NULL;
    if ( f_rena ) {
        if ( rena != NULL ) {
            s_rena = s_alloc( A-> A_nQ );
            for( i = A-> A_nQ; --i >= 0; ) {
                if ( rena[i] < A-> A_nQ )
                    s_rena[i] = trena[rena[i]];
                else    s_rena[i] = MAXSHORT;
            }
        } else {
            s_rena = trena;
            trena = NULL;
        }
    }
    Sfree( (char *) trena );

    A-> A_nrows = pz - A-> A_t;
    A-> A_nQ = nrena;
    return( A_close( A_open( A ) ) );
}

A_OBJECT A_mkdense( A )
register A_OBJECT A;
{
    register A_row *p, *pz;
    register R_OBJECT R;

    if ( A == NULL ) Error( "A_rename: No OBJECT" );

    R = R_create();
    R_insert( R, 0, 0 );
    R_insert( R, 1, 0 );
    pz = A-> A_t + A-> A_nrows;
    for( p = A-> A_t; p < pz; ++p ) {
      p-> A_a = R_insert( R, p-> A_a, 0 );
      p-> A_c = R_insert( R, p-> A_c, 0 );
    }
    A-> A_nQ = R-> R_n;
    R_destroy( R );
    return( A_close( A_open( A ) ) );
}
