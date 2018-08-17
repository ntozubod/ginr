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
#include <string.h>

extern FILE *fpout;

#include "O.h"

static char *T_nmeptr = 0;
static SHORT *T_hashpos = 0;
static int T_calls = 0;
static int T_probes = 0;
static int T_fail = 0;

T_OBJECT T_create( )
{
        register T_OBJECT T;
        T = (T_OBJECT) Salloc( sizeof(struct T_desc) );
        T-> Type = T_Object;
        T-> T_n  = 0;
        T-> T_lname = 0;
        T-> T_lhash = 1;
        T-> T_name = 0;
        T-> T_hash = s_alloc( 1 );
        T-> T_hash[ 0 ] = MAXSHORT;
        return( T );
}

void T_destroy( T )
register T_OBJECT T;
{
        register char **p, **pl;
        if ( T == NULL ) return;
        p = T-> T_name;
        pl = p + T-> T_n;
        while ( p < pl ) Sfree( (char *) *p++ );
        Sfree( (char *) T-> T_name );
        Sfree( (char *) T-> T_hash );
        Sfree( (char *) T );
}

int T_member( T, name )
register T_OBJECT T;
char *name;
{
        register int h;
        register char *na;
        register SHORT *p;
        ++T_calls;
        h = 0;
        for ( na = name; *na; ) h = ( ( h + *na++ ) * 16807 ) & 017777777777;
        p  = T-> T_hash + h % T-> T_lhash;
        while ( *p < MAXSHORT ) {
                ++T_probes;
                if ( strcmp( T-> T_name[*p], name ) == 0 ) return( *p );
                if ( --p < T-> T_hash )
                        p = T-> T_hash + T-> T_lhash - 1;
        }
        ++T_fail;
        T_nmeptr = na;
        T_hashpos = p;
        return( -1 );
}

T_OBJECT T_grow( T, lname )
register T_OBJECT T;
int lname;
{
        register SHORT *p, *pl;
        register char **q, **ql;
        register int i;
        if ( lname < 15 ) lname = 15;
        if ( lname <= T-> T_lname ) return( T );
        Sfree( (char *) T-> T_hash );
        T-> T_name =
                (char **) Srealloc( (char *) T-> T_name,
                        lname * sizeof(char *) );
        T-> T_lname = Ssize( (char *) T-> T_name ) / sizeof(char *);
        if ( T-> T_lname > MAXSHORT ) T-> T_lname = MAXSHORT;
        T-> T_hash = s_alloc( 2 * T-> T_lname );
        T-> T_lhash = Ssize( (char *) T-> T_hash ) / sizeof(SHORT);
        p = T-> T_hash;
        pl = p + T-> T_lhash;
        while ( p < pl ) *p++ = MAXSHORT;
        q = T-> T_name;
        ql = q + T-> T_n;
        i = 0;
        while ( q < ql ) {
                if ( T_member( T, *q++ ) != (-1) )
                        Error( "T_grow: BOTCH" );
                *T_hashpos = i++;
        }
        return( T );
}

int T_insert( T, name )
register T_OBJECT T;
register char *name;
{
        register int i;
        if ( T-> T_n >= T-> T_lname ) {
                if ( T-> T_n >= MAXSHORT )
                        Error( "T_insert: Table FULL" );
                T = T_grow( T, 2 * T-> T_lname );
        }
        if ( (i = T_member( T, name )) >= 0 ) return( i );
        T-> T_name[ T-> T_n ]
                = strcpy( Salloc( T_nmeptr - name + 1 ), name );
        return( *T_hashpos = T-> T_n++ );
}

char *T_name( T, i )
register T_OBJECT T;
register int i;
{
        if ( i >= 0 && i < T-> T_n ) return( T-> T_name[ i ] );
        else    return( NULL );
}

void T_stats()
{
        fprintf( fpout, "(T) Calls:%7d  Probes:%7d  Unsuccessful:%7d\n",
                T_calls, T_probes, T_fail );
}
