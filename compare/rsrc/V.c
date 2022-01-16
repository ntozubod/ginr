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

static SHORT *V_vecptr = 0;
static SHORT *V_hashpos = 0;
static int V_calls = 0;
static int V_probes = 0;
static int V_fail = 0;

SHORT *veccpy( SHORT *p, SHORT *q )
{
    register SHORT *save = p;

    while ( *q < MAXSHORT ) {
        *p++ = *q++;
    }

    *p = *q;
    return ( save );
}

int veccmp( SHORT *p, SHORT *q )
{
    --p;
    --q;

    while ( *++p == *++q ) {

        if ( *p == MAXSHORT ) {
            return ( 0 );
        }
    }

    return ( *p - *q );
}

int veclen( register SHORT *p )
{
    register int i = 0;

    while ( *p++ < MAXSHORT ) {
        ++i;
    }

    return ( i );
}

V_OBJECT V_create( )
{
    register V_OBJECT V;
    V = (V_OBJECT) Salloc( sizeof(struct V_desc) );
    V-> Type = V_Object;
    V-> V_n  = 0;
    V-> V_lvec = 0;
    V-> V_lhash = 1;
    V-> V_vec = 0;
    V-> V_hash = s_alloc( 1 );
    V-> V_hash[ 0 ] = MAXSHORT;
    return ( V );
}

void V_destroy( register V_OBJECT V )
{
    register SHORT **p, **pl;

    if ( V == NULL ) {
        return;
    }

    p = V-> V_vec;
    pl = p + V-> V_n;

    while ( p < pl ) {
        Sfree( (char *) *p++ );
    }

    Sfree( (char *) V-> V_vec );
    Sfree( (char *) V-> V_hash );
    Sfree( (char *) V );
}

int V_member( register V_OBJECT V, SHORT *vec )
{
    register int h;
    register SHORT *na;
    register SHORT *p;
    ++V_calls;
    h = 0;

    for (   na = vec;
            *na < MAXSHORT; ) {
        h = ( ( h + *na++ ) * 16807 ) & 017777777777;
    }

    p  = V-> V_hash + h % V-> V_lhash;

    while ( *p < MAXSHORT ) {
        ++V_probes;

        if ( veccmp( V-> V_vec[*p], vec ) == 0 ) {
            return ( *p );
        }

        if ( --p < V-> V_hash ) {
            p = V-> V_hash + V-> V_lhash - 1;
        }
    }

    ++V_fail;
    V_vecptr = na;
    V_hashpos = p;
    return ( -1 );
}

V_OBJECT V_grow( register V_OBJECT V, int lvec )
{
    register SHORT *p, *pl;
    register SHORT **q, **ql;
    register int i;

    if ( lvec < 15 ) {
        lvec = 15;
    }

    if ( lvec <= V-> V_lvec ) {
        return ( V );
    }

    Sfree( (char *) V-> V_hash );
    V-> V_vec =
        (SHORT **) Srealloc( (char *) V-> V_vec,
                             lvec * sizeof(SHORT *) );
    V-> V_lvec = Ssize( (char *) V-> V_vec ) / sizeof(SHORT *);

    if ( V-> V_lvec > MAXSHORT ) {
        V-> V_lvec = MAXSHORT;
    }

    V-> V_hash = s_alloc( 2 * V-> V_lvec );
    V-> V_lhash = Ssize( (char *) V-> V_hash ) / sizeof(SHORT);
    p = V-> V_hash;
    pl = p + V-> V_lhash;

    while ( p < pl ) {
        *p++ = MAXSHORT;
    }

    q = V-> V_vec;
    ql = q + V-> V_n;
    i = 0;
    while ( q < ql ) {

        if ( V_member( V, *q++ ) != (-1) ) {
            Error( "V_grow: BOTCH" );
        }

        *V_hashpos = i++;
    }

    return ( V );
}

int V_insert( register V_OBJECT V, register SHORT *vec )
{
    register int i;

    if ( V-> V_n >= V-> V_lvec ) {

        if ( V-> V_n >= MAXSHORT ) {
            Error( "V_insert: Table FULL" );
        }

        V = V_grow( V, 2 * V-> V_lvec );
    }

    if ( (i = V_member( V, vec )) >= 0 ) {
        return ( i );
    }

    V-> V_vec[ V-> V_n ] = veccpy( s_alloc( V_vecptr - vec + 1 ), vec );
    return ( *V_hashpos = V-> V_n++ );
}

SHORT *V_vec( register V_OBJECT V, register int i )
{

    if ( i >= 0 && i < V-> V_n ) {
        return ( V-> V_vec[ i ] );
    }

    else {
        return ( NULL );
    }
}

void V_stats()
{
    fprintf( fpout, "(V) Calls:%7d  Probes:%7d  Unsuccessful:%7d\n",
             V_calls, V_probes, V_fail );
}
