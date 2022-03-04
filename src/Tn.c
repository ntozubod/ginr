/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Copyright (c) 2022, J Howard Johnson
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

static SHORT *Tn_hashpos = 0;
static int Tn_calls = 0;
static int Tn_probes = 0;
static int Tn_fail = 0;

Tn_OBJECT Tn_create( )
{
    Tn_OBJECT Tn;
    Tn = (Tn_OBJECT) Salloc( sizeof(struct Tn_desc) );
    Tn-> Type = Tn_Object;
    Tn-> Tn_n  = 0;
    Tn-> Tn_lname = 0;
    Tn-> Tn_lhash = 1;
    Tn-> Tn_lstor = 100;

    Tn-> Tn_idxs = (int *) Salloc( 100 * sizeof( int ) );
    Tn-> Tn_idxs[ 0 ] = 0;
    Tn-> Tn_hash = s_alloc( 1 );
    Tn-> Tn_hash[ 0 ] = MAXSHORT;
    Tn-> Tn_stor = Salloc( 100 );

    return( Tn );
}

void Tn_destroy( Tn_OBJECT Tn )
{
    if ( Tn == NULL ) return;

    Sfree( (char *) Tn-> Tn_idxs );
    Sfree( (char *) Tn-> Tn_hash );
    Sfree( (char *) Tn-> Tn_stor );
    Sfree( (char *) Tn );
}

int Tn_member( Tn_OBJECT Tn, char *name, int length )
{
    int h, i, k, len_k;
    char *na;
    SHORT *p;
    assert( name[ length ] == '\0' );
    ++Tn_calls;
    h = 0;
    for ( i = 0; i < length; ++i ) {
        h = ( ( h + (unsigned) name[ i ] ) * 16807 ) & 017777777777;
    }
    p  = Tn-> Tn_hash + h % Tn-> Tn_lhash;
    while ( *p < MAXSHORT ) {
        ++Tn_probes;
        k = *p;
        len_k = Tn-> Tn_idxs[ k + 1 ] - Tn-> Tn_idxs[ k ] - 1;
        if ( length == len_k ) {
            na = Tn-> Tn_stor + Tn-> Tn_idxs[ k ];
            for ( i = 0; i < length; ++i ) {
                if ( na[ i ] != name[ i ] ) break;
            }
            if ( i == length ) return( k );
        }
        if ( --p < Tn-> Tn_hash )
            p = Tn-> Tn_hash + Tn-> Tn_lhash - 1;
    }
    ++Tn_fail;
    Tn_hashpos = p;
    return( -1 );
}

Tn_OBJECT Tn_grow( Tn_OBJECT Tn, int lname )
{
    int i, idx, idx_next, len;

    if ( lname < 15 ) lname = 15;
    if ( lname <= Tn-> Tn_lname ) return( Tn );

    Sfree( (char *) Tn-> Tn_hash );

    Tn-> Tn_idxs =
        (int *) Srealloc( (char *) Tn-> Tn_idxs, ( lname + 1 ) * sizeof(int) );
    Tn-> Tn_lname = Ssize( (char *) Tn-> Tn_idxs ) / sizeof(int) - 1;
    if ( Tn-> Tn_lname > MAXSHORT ) Tn-> Tn_lname = MAXSHORT;

    Tn-> Tn_hash = s_alloc( 2 * Tn-> Tn_lname );
    Tn-> Tn_lhash = Ssize( (char *) Tn-> Tn_hash ) / sizeof(SHORT);
    for( i = 0; i < Tn-> Tn_lhash; ++i ) {
        Tn-> Tn_hash[ i ] = MAXSHORT;
    }

    idx_next = Tn-> Tn_idxs[ 0 ];
    assert( idx_next == 0 );
    
    for( i = 0; i < Tn-> Tn_n; ++i ) {
        idx = idx_next;
        idx_next = Tn-> Tn_idxs[ i + 1 ];
        len = idx_next - idx - 1;
        if ( Tn_member( Tn, Tn-> Tn_stor + idx, len ) != (-1) )
            Error( "Tn_grow: BOTCH" );
        *Tn_hashpos = i;
    }
    return( Tn );
}

int Tn_insert( Tn_OBJECT Tn, char *name, int length )
{
    int i, k, current_stor_size, next_stor_size;
    char *na;
    assert( name[ length ] == '\0' );
    if ( Tn-> Tn_n >= Tn-> Tn_lname ) {
        if ( Tn-> Tn_n >= MAXSHORT )
            Error( "Tn_insert: Table FULL" );
        Tn = Tn_grow( Tn, 2 * Tn-> Tn_lname );
    }
    
    if ( (i = Tn_member( Tn, name, length )) >= 0 ) return( i );

    k = Tn-> Tn_n;
    current_stor_size = Tn-> Tn_idxs[ k ];
    next_stor_size = current_stor_size + length + 1;
    if ( next_stor_size > Tn-> Tn_lstor ) {
        Tn-> Tn_stor = Srealloc( Tn-> Tn_stor, 2 * next_stor_size );
    }

    na = Tn-> Tn_stor + current_stor_size;
    for( i = 0; i < length; ++i ) {
        na[ i ] = name[ i ];
    }
    na[ length ] = '\0';

    Tn-> Tn_idxs[ k + 1 ] = next_stor_size;
    return( *Tn_hashpos = Tn-> Tn_n++ );
}

char *Tn_name( Tn_OBJECT Tn, int i )
{
    if ( i >= 0 && i < Tn-> Tn_n )
            return( Tn-> Tn_stor + Tn-> Tn_idxs[ i ] );
    else    return( NULL );
}

int Tn_length( Tn_OBJECT Tn, int i )
{
    if ( i >= 0 && i < Tn-> Tn_n )
            return( Tn-> Tn_idxs[ i + 1 ] - Tn-> Tn_idxs[ i ] - 1 );
    else    return( -1 );
}

P_OBJECT Tn_Pstr( Tn_OBJECT Tn, int i )
{
    if ( i >= 0 && i < Tn-> Tn_n )
        return(
            P_create(
                Tn-> Tn_idxs[ i + 1 ] - Tn-> Tn_idxs[ i ] - 1,
                Tn-> Tn_stor + Tn-> Tn_idxs[ i ] ) );
    else    return( NULL );
}

void Tn_stats()
{
    fprintf( fpout, "(Tn) Calls:%7d  Probes:%7d  Unsuccessful:%7d\n",
             Tn_calls, Tn_probes, Tn_fail );
}
