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

#define UNMARK          MAXSHORT
#define DELETED         (MAXSHORT-1)

static A_OBJECT         GAt;
static SHORT            *t_stk, *t_low;
static int              t_cnt;

/*
 * A_trim does a deep recursion that reaches a depth of the maximum path
 * length from the start state. Each frame has 2 SHORTs and a pointer
 * (12 bytes).
 *
 * SHORT state
 * SHORT dfn
 * A_row * p
 *
 * t_stk SHORT ptr
 * t_low SHORT ptr
 * A_p ptr     array
 *
 * Stack: with SHORT having 4 bytes now 4 + 4 + 8 = 16
 * Heap:  2 SHORTs and a ptr  = 16 * A_nQ
 * total cost 4 * 4 + 8 * 2 = 32 * A_nQ

 * two stacks:
 * 
 *  rec_stk_idx   recursion stack -- replacing C stack
 *  cmp_stk_idx   component stack -- replacing old t_stk

 * dfn[ rec_stk_idx ]
 * state[ rec_stk_idx ]
 * p[ rec_stk_idx ]

 * comp[ cmp_stk_idx ]  replaces *t_stk

 * low[ <state>  ] contains lowest dfn for component containing <state>

 * =================================================
 * The old recursive implementation to be replaced:
 * =================================================
 */

#ifdef USE_RECURSIVE_TRIM

SHORT A_tr_DFS( SHORT state )
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

#else

SHORT A_tr_DFS( SHORT l_state )
{
    SHORT *state   = s_alloc( GAt-> A_nQ );
    SHORT *dfn     = s_alloc( GAt-> A_nQ );
    A_row **p      = (A_row **) Salloc( GAt-> A_nQ * sizeof(A_row *) );
    SHORT *comp    = t_stk;
    SHORT *low     = t_low;

    SHORT l_dfn;
    A_row *l_p;
    SHORT l_next;
    SHORT return_val;

    int rec_stk_idx = 0;
    int cmp_stk_idx = 0;

A_tr_DFS_call_label:
    state[ rec_stk_idx ] = l_state;
    l_dfn = t_cnt++;
    dfn[ rec_stk_idx ] = l_dfn;
    low[ l_state ] = l_dfn;
    comp[ cmp_stk_idx++ ] = l_state;

    l_p = GAt-> A_p[ l_state + 1 ];
    p[ rec_stk_idx ] = l_p;

A_tr_DFS_p_loop_label:
    l_p = --p[ rec_stk_idx ];
    l_state = state[ rec_stk_idx ];
    if ( l_p < GAt-> A_p[ l_state ] )
        goto A_tr_DFS_exit_p_loop_label;

    l_next = l_p-> A_c;
    if ( low[ l_next ] == UNMARK ) {
        l_state = l_next;
        ++rec_stk_idx;
        goto A_tr_DFS_call_label;
    } else goto A_tr_DFS_skip;

A_tr_DFS_return_label:
    --rec_stk_idx;
    l_next = return_val;

A_tr_DFS_skip:
    l_state = state[ rec_stk_idx ];
    if ( low[ l_next ] < low[ l_state ] ) {
        low[ l_state ] = low[ l_next ];
    }
    goto A_tr_DFS_p_loop_label;

A_tr_DFS_exit_p_loop_label:
    l_state = state[ rec_stk_idx ];
    l_dfn = dfn[ rec_stk_idx ];
    if ( low[ l_state ] == l_dfn ) {
        for( l_next = MAXSHORT; l_next != l_state; ) {
            l_next = comp[ --cmp_stk_idx ];
            low[ l_next ] = DELETED;
        }
    }

    l_state = state[ rec_stk_idx ];
    return_val = l_state;

    if ( rec_stk_idx > 0 )
        goto A_tr_DFS_return_label;

    Sfree( (char *) state );
    Sfree( (char *) dfn );
    Sfree( (char *) p );
    return( return_val );
}

#endif

A_OBJECT A_trim( A_OBJECT A )
{
    A_row  *p, *pz;
    int    i, must_reopen;
    SHORT  *stk_rem;

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
