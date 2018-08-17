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

#define UNMARK          MAXSHORT
#define DELETED         (MAXSHORT-1)

static A_OBJECT         GAt;
static SHORT            *t_stk, *t_low;
static int              t_cnt;

SHORT A_tr_DFS( state )
SHORT state;
{
        SHORT           dfn;
        A_row           *p;
        static SHORT    next;

        t_low[ state ] = dfn = t_cnt++;
        *t_stk++ = state;
        for( p = GAt-> A_p[state+1]; --p >= GAt-> A_p[state]; ) {
                if ( t_low[ next = p-> A_c ] == UNMARK )
                        next = A_tr_DFS( next );
                if ( t_low[next] < t_low[state] )
                        t_low[state] = t_low[next];
        }
        if ( t_low[state] == dfn )
                for( next = MAXSHORT; next != state; )
                        t_low[ next = *--t_stk ] = DELETED;
        return( state );
}

A_OBJECT A_trim( A )
register A_OBJECT A;
{
        register A_row  *p, *pz;
        register int    i, must_reopen;
        SHORT           *stk_rem;

        if ( A == NULL ) Error( "A_trim: No OBJECT" );
        if ( A-> A_nQ >= DELETED ) Error( "A_trim: Too many states" );
        if ( A-> A_mode <  NFA ) A = A_close( A );
        if ( A-> A_mode >= NFA_TRIM ) return( A );
        if ( A-> A_nrows == 0 ) {
                A-> A_mode = NFA_TRIM;
                return( A );
        }

        if ( A_report ) fprintf( fpout, "--> A_trim\n" );
        t_stk = stk_rem = s_alloc( A-> A_nQ );
        t_low = s_alloc( A-> A_nQ );
        for( i = A-> A_nQ; --i >= 0; ) t_low[i] = UNMARK;
        t_low[ FINAL ] = 0;
        t_cnt = 1;
        GAt = A;
        if ( A_tr_DFS( (SHORT) START ) != START )
                Error( "A_trim: BOTCH" );
        Sfree( (char *) stk_rem );
        t_low[ FINAL ] = t_low[ START ];
        must_reopen = 0;
        for( p = pz = A-> A_t + A-> A_nrows; --p >= A-> A_t; )
                if ( t_low[ p-> A_a ] >= DELETED
                  || t_low[ p-> A_c ] >= DELETED ) {
                        --pz;
                        p-> A_a = pz-> A_a;
                        p-> A_b = pz-> A_b;
                        p-> A_c = pz-> A_c;
                        must_reopen = 1;
                }
        A-> A_nrows = pz - A-> A_t;
        Sfree( (char *) t_low );
        if ( must_reopen ) A = A_close( A_open( A ) );
        A-> A_mode = NFA_TRIM;
        for( p = pz = A-> A_t + A-> A_nrows; --p >= A-> A_t; )
                if ( p-> A_b == 0 ) break;
        if ( p < A-> A_t ) {
                A-> A_mode = NFA_CLOSED;
                for( p = pz; --p > A-> A_t; )
                        if ( (p-1)-> A_b == p-> A_b
                          && (p-1)-> A_a == p-> A_a ) break;
                if ( p == A-> A_t )
                        A-> A_mode = DFA;
        }
        if ( A_report ) {
                fprintf( fpout, "<-- A_trim  " );
                (void) A_rept( A );
        }
        return( A );
}
