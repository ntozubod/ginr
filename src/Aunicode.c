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

A_OBJECT A_slurp_octets( char *file, T_OBJECT T_Sigma )
{
    A_OBJECT A;
    FILE *fp;
    int c;
    SHORT state, next_state;

    if ( file != NULL ) fp = fopen( file, "r" );
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }

    assert( T_Sigma != NULL );
    assert( T_Sigma-> T_n >= 258 + 32 );

    A = A_create();
    c = getc( fp );
    state = 0;
    next_state = 2;
    while ( c != EOF ) {
        assert( next_state <= MAXSHORT );
        A = A_add( A, state, c + 2, next_state );
        state = next_state++;
        c = getc( fp );
    }
    A = A_add( A, state, 1, 1 );
    A = A_close( A );
    fclose( fp );
    return( A );
}

A_OBJECT A_slurp_nibbles( char *file, T_OBJECT T_Sigma )
{
    A_OBJECT A;
    FILE *fp;
    int c;
    SHORT state, state1, state2;

    if ( file != NULL ) fp = fopen( file, "r" );
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }

    assert( T_Sigma != NULL );

    A = A_create();
    c = getc( fp );
    state = 0;
    state1 = 2;
    state2 = 3;
    while ( c != EOF ) {
        assert( state2 <= MAXSHORT );
        A = A_add( A, state,  ( c >> 4 ) + 2 + 256, state1 );
        A = A_add( A, state1, ( c & 0xf) + 2 + 256 + 16, state2 );
        state = state2;
        state1 = state + 1;
        state2 = state + 2;
        c = getc( fp );
    }
    A = A_add( A, state, 1, 1 );
    A = A_close( A );
    fclose( fp );
    return( A );
}

A_OBJECT A_slurp_utf8( char *file, T_OBJECT T_Sigma )
{
    A_OBJECT A;
    FILE *fp;
    int i, c1, c2, c3, c4, type;
    char ts[ 5 ];

    if ( file != NULL ) fp = fopen( file, "r" );
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }

    assert( T_Sigma != NULL );
    assert( T_Sigma-> T_n >= 258 + 32 );

    A = A_slurp_octets( file, T_Sigma );
    A = A_open( A );

    for ( i = 0; i < A-> A_nrows; ++i ) {
        c1 = A-> A_t[ i ].A_b - 2;
             if ( ( c1 & 0x80 ) == 0x00 ) { type = 0; }
        else if ( ( c1 & 0xc0 ) == 0x80 ) { type = 1; }
        else if ( ( c1 & 0xe0 ) == 0xc0 ) { type = 2; }
        else if ( ( c1 & 0xf0 ) == 0xe0 ) { type = 3; }
        else if ( ( c1 & 0xf8 ) == 0xf0 ) { type = 4; }
        else { type = 5; }

        switch ( type ) {

            case 2:
                c2 = A-> A_t[ i + 1 ].A_b - 2;
                if ( ( c2 & 0xc0 ) == 0x80 ) {
                    ts[ 0 ] = c1;
                    ts[ 1 ] = c2;
                    ts[ 2 ] = '\0';
                }
                A-> A_t[ i ].A_b = T_insert( T_Sigma, ts );
                A-> A_t[ i ].A_c += 1;
                break;

            case 3:
                c2 = A-> A_t[ i + 1 ].A_b - 2;
                c3 = A-> A_t[ i + 2 ].A_b - 2;
                if (    ( c2 & 0xc0 ) == 0x80
                     && ( c3 & 0xc0 ) == 0x80 ) {
                    ts[ 0 ] = c1;
                    ts[ 1 ] = c2;
                    ts[ 2 ] = c3;
                    ts[ 3 ] = '\0';
                }
                A-> A_t[ i ].A_b = T_insert( T_Sigma, ts );
                A-> A_t[ i ].A_c += 2;
                break;

            case 4:
                c2 = A-> A_t[ i + 1 ].A_b - 2;
                c3 = A-> A_t[ i + 2 ].A_b - 2;
                c4 = A-> A_t[ i + 3 ].A_b - 2;
                if (    ( c2 & 0xc0 ) == 0x80
                     && ( c3 & 0xc0 ) == 0x80
                     && ( c4 & 0xc0 ) == 0x80 ) {
                    ts[ 0 ] = c1;
                    ts[ 1 ] = c2;
                    ts[ 2 ] = c3;
                    ts[ 3 ] = c4;
                    ts[ 4 ] = '\0';
                }
                A-> A_t[ i ].A_b = T_insert( T_Sigma, ts );
                A-> A_t[ i ].A_c += 3;
                break;

            case 0: case 1: case 5:
                break;
        }
    }

    if ( T_Sigma-> T_n > A-> A_nS ) {
        A-> A_nS = T_Sigma-> T_n;
    }
    A = A_min( A );
    fclose( fp );
    return( A );
}

A_OBJECT A_spit_octets( A_OBJECT A, char *file, T_OBJECT T_Sigma )
{
    FILE *fp;
    int i, s1, c1;

    if ( file != NULL ) fp = fopen( file, "w" );
    if ( fp == NULL ) {
        Warning( "Can't open file for write" );
        return( NULL );
    }

    assert( T_Sigma != NULL );
    assert( T_Sigma-> T_n >= 258 + 32 );

    A = A_min( A );

    for ( i = 0; i < A-> A_nrows; ++i ) {
        s1 = A-> A_t[ i ].A_b;
        assert( s1 < 258 );
        if ( s1 >= 2 ) {
            c1 = s1 - 2;
            fputc( c1, fp );
        }
    }
    fclose( fp );
    return( A );
}

A_OBJECT A_spit_nibbles( A_OBJECT A, char *file, T_OBJECT T_Sigma )
{
    FILE *fp;
    int i, s1, c1;
    int accum = -1;

    if ( file != NULL ) fp = fopen( file, "w" );
    if ( fp == NULL ) {
        Warning( "Can't open file for write" );
        return( NULL );
    }

    assert( T_Sigma != NULL );
    assert( T_Sigma-> T_n >= 258 + 32 );

    A = A_min( A );

    for ( i = 0; i < A-> A_nrows; ++i ) {
        s1 = A-> A_t[ i ].A_b;
        assert( s1 < 258 + 32 );
        if ( s1 >= 2 ) {
            assert( s1 >= 258 );
            c1 = s1 - 258;
            if ( c1 < 16 ) {
                assert( accum == -1 );
                accum = c1 << 4;
            } else {
                assert( c1 < 32 );
                fputc( accum + c1 - 16, fp );
                accum = -1;
            }
        }
    }
    assert( accum == -1 );
    fclose( fp );
    return( A );
}

A_OBJECT A_spit_utf8( A_OBJECT A, char *file, T_OBJECT T_Sigma )
{
    FILE *fp;
    int i, s1, c1, type;

    if ( file != NULL ) fp = fopen( file, "w" );
    if ( fp == NULL ) {
        Warning( "Can't open file for write" );
        return( NULL );
    }

    assert( T_Sigma != NULL );
    assert( T_Sigma-> T_n >= 258 + 32 );

    A = A_min( A );

    for ( i = 0; i < A-> A_nrows; ++i ) {
        s1 = A-> A_t[ i ].A_b;
        if ( s1 < 2 ) {
        } else if ( s1 < 258 ) {
            c1 = s1 - 2;
                 if ( ( c1 & 0x80 ) == 0x00 ) { type = 0; }
            else if ( ( c1 & 0xc0 ) == 0x80 ) { type = 1; }
            else if ( ( c1 & 0xe0 ) == 0xc0 ) { type = 2; }
            else if ( ( c1 & 0xf0 ) == 0xe0 ) { type = 3; }
            else if ( ( c1 & 0xf8 ) == 0xf0 ) { type = 4; }
            else { type = 5; }

            switch ( type ) {
                case 0: case 1: case 2: case 3: case 4: case 5:
                    fputc( c1, fp );
                    break;
            }
        } else {
            fputs( T_name( T_Sigma, s1 ), fp );
        }
    }
    fclose( fp );
    return( A );
}

A_OBJECT A_gen_min( A_OBJECT A )
{
    int i, dead_state, last_state, state;

    A = A_lenmin( A );
    A = A_min( A );
    A = A_open( A );
    dead_state = A-> A_nQ++;
    last_state = -1;
    for ( i = 0; i < A-> A_nrows; ++i ) {
        state  = A-> A_t[ i ].A_a;
        if ( state == last_state ) {
            A-> A_t[ i ].A_c = dead_state;
        }
        last_state = state;
    }
    A = A_min( A );
    return( A );
}
