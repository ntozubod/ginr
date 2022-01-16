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

#define INIT_lrows  20

int A_report = 0;
extern FILE *fpout;

A_OBJECT A_create()
{
    register A_OBJECT A;

    A =           (A_OBJECT) Salloc( sizeof(struct A_desc) );
    A-> Type =    A_Object;
    A-> A_mode =  OPEN;
    A-> A_ems =   0;
    A-> A_nT =    1;
    A-> A_nQ =    2;
    A-> A_nS =    2;
    A-> A_nrows = 0;
    A-> A_p =     NULL;
    A-> A_t =     (A_row *) Salloc( ( INIT_lrows + 2 ) * sizeof(A_row) );
    A-> A_lrows = Ssize( (char *) A-> A_t ) / ( sizeof(A_row) ) - 2;

    return( A );
}

void A_destroy( register A_OBJECT A )
{

    if ( A != NULL ) {
        Sfree( (char *) A-> A_p );
        Sfree( (char *) A-> A_t );
    }

    Sfree( (char *) A );
}

A_OBJECT A_rept( register A_OBJECT A )
{

    if ( A == NULL ) {
        fprintf( fpout, "  NULL Automaton\n" );
        return( A );
    }

    if ( A-> A_nrows == 0 ) {
        fprintf( fpout, "  Empty Automaton\n" );
        return( A );
    }

    if ( A-> A_ems ) {
        fprintf( fpout, "*" );
    }

    else {
        fprintf( fpout, " " );
    }

    switch ( A-> A_mode ) {
    case OPEN:
        fprintf( fpout, "UPDATE  " );
        break;
    case NFA:
        fprintf( fpout, "NFA     " );
        break;
    case NFA_TRIM:
        fprintf( fpout, "TRIM    " );
        break;
    case NFA_EQLAM:
        fprintf( fpout, "LAM EQ  " );
        break;
    case NFA_CMLAM:
        fprintf( fpout, "LAM CM  " );
        break;
    case NFA_CLOSED:
        fprintf( fpout, "CLOSED  " );
        break;
    case DFA:
        fprintf( fpout, "DFA     " );
        break;
    case DFA_MIN:
        fprintf( fpout, "DFA MIN " );
        break;
    case SSEQ:
        fprintf( fpout, "SSEQ    " );
        break;
    case SSEQ_MIN:
        fprintf( fpout, "SSEQMIN " );
        break;
    }

    fprintf( fpout, "States: %-6d Trans: %-6d", A-> A_nQ, A-> A_nrows );
    fprintf( fpout, " Tapes: %-2d", A-> A_nT );
    fprintf( fpout, " Strg: %ld K",
             ( Ssize( (char *) A )
               + ( A-> A_mode == OPEN ? 0 : Ssize( (char *) A-> A_p ) )
               + Ssize( (char *) A-> A_t ) + 1023 ) / 1024 );
    fprintf( fpout, "\n" );
    fflush( fpout );
    return( A );
}

void A_exchange( register A_OBJECT A1, register A_OBJECT A2 )
{
    register int      t_int;
    register A_row ** t_Arpp;
    register A_row *  t_Arp;

    if ( A1 == NULL ) {
        A1 = A_create();
        A1-> A_mode = (-1);
    }

    if ( A2 == NULL ) {
        A2 = A_create();
        A2-> A_mode = (-1);
    }

    t_int = A2-> A_mode;
    A2-> A_mode = A1-> A_mode;
    A1-> A_mode = t_int;
    t_int = A2-> A_ems;
    A2-> A_ems = A1-> A_ems;
    A1-> A_ems = t_int;
    t_int = A2-> A_nT;
    A2-> A_nT = A1-> A_nT;
    A1-> A_nT = t_int;
    t_int = A2-> A_nQ;
    A2-> A_nQ = A1-> A_nQ;
    A1-> A_nQ = t_int;
    t_int = A2-> A_nS;
    A2-> A_nS = A1-> A_nS;
    A1-> A_nS = t_int;
    t_int = A2-> A_nrows;
    A2-> A_nrows = A1-> A_nrows;
    A1-> A_nrows = t_int;
    t_int = A2-> A_lrows;
    A2-> A_lrows = A1-> A_lrows;
    A1-> A_lrows = t_int;
    t_Arpp = A2-> A_p;
    A2-> A_p = A1-> A_p;
    A1-> A_p = t_Arpp;
    t_Arp = A2-> A_t;
    A2-> A_t = A1-> A_t;
    A1-> A_t = t_Arp;

    if ( A1-> A_mode == (-1) ) {
        A_destroy( A1 );
    }

    if ( A2-> A_mode == (-1) ) {
        A_destroy( A2 );
    }
}

A_OBJECT A_copy( register A_OBJECT A )
{
    register int i;
    register A_OBJECT newA;

    if ( A == NULL ) {
        return( NULL );
    }

    newA = (A_OBJECT) Scopy( (char *) A );
    newA-> A_p = NULL;
    newA-> A_t = (A_row *) Scopy( (char *) A-> A_t );

    if ( A-> A_mode != OPEN ) {
        newA-> A_p = (A_row **) Salloc( Ssize( (char *) A-> A_p ) );

        for (   i = 0;
                i <= A-> A_nQ;
                i++ ) {
            newA-> A_p[i] = newA-> A_t + (A-> A_p[i] - A-> A_t);
        }
    }

    return( newA );
}

A_OBJECT A_deems( register A_OBJECT A )
{
    int new_mode;
    int lst_em;
    register A_row *p;

    if ( !( A-> A_ems ) ) {
        return A;
    }

    lst_em = 2 * A-> A_nT - 1;

    if ( A-> A_mode < NFA_EQLAM ) {
        new_mode = A-> A_mode;
    }

    else {
        new_mode = NFA_EQLAM;
    }

    A = A_open( A );

    for ( p = A-> A_t + A-> A_nrows;
            --p >= A-> A_t; ) {

        if ( p-> A_b > 1 && p-> A_b <= lst_em ) {
            p-> A_b = 0;
        }
    }

    A = A_close( A );
    A-> A_mode = new_mode;
    A-> A_ems = 0;
    return A;
}

A_OBJECT A_adems( register A_OBJECT A )
{
    int new_mode;
    int fst_em, lst_em;
    register int i;
    register int base;
    register A_row *p;

    if ( A-> A_ems ) {
        return A;
    }

    if ( A-> A_nT == 1 ) {
        Error( "Can't add endmarkers to one tape automaton" );
    }

    fst_em = A-> A_nT;
    lst_em = 2 * A-> A_nT - 1;

    if ( A-> A_mode < DFA_MIN ) {
        new_mode = A-> A_mode;
    }

    else {
        new_mode = DFA_MIN;
    }

    A = A_open( A );
    base = A-> A_nQ;

    for (   p = A-> A_t + A-> A_nrows;
            --p >= A-> A_t; ) {

        if ( p-> A_b == 1 ) {
            p-> A_b = fst_em;
            p-> A_c = base;
        }
    }

    for (   i = fst_em + 1;
            i <= lst_em;
            i++ ) {
        A = A_add( A, base + i - fst_em - 1, i, base + i - fst_em );
    }

    A = A_add( A, base + lst_em - fst_em, 1, FINAL );
    A = A_close( A );
    A-> A_mode = new_mode;
    A-> A_ems = 1;
    return A;
}
