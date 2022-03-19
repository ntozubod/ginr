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

A_OBJECT A_slurp_octets( char *file, T2_OBJECT T2_Sigma )
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

    assert( T2_Sigma != NULL );
    assert( T2_Sigma-> T2_int-> Tn_n >= 258 );

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

A_OBJECT A_slurp_nibbles( char *file, T2_OBJECT T2_Sigma )
{
    A_OBJECT A;
    FILE *fp;
    int c;
    SHORT state, state1, state2, state3;
    int T2idx[] = { 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
                    67, 68, 69, 70, 71, 72 };
    int T2idx_blk =  97;

    if ( file != NULL ) fp = fopen( file, "r" );
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }

    assert( T2_Sigma != NULL );

    A = A_create();
    c = getc( fp );
    state = 0;
    state1 = 2;
    state2 = 3;
    state3 = 4;
    while ( c != EOF ) {
        assert( state3 <= MAXSHORT );
        A = A_add( A, state,  T2idx[ ( c >> 4 ) & 0xf ], state1 );
        A = A_add( A, state1, T2idx[ c & 0xf ], state2 );
        A = A_add( A, state2, T2idx_blk, state3 );
        state = state3;
        state1 = state + 1;
        state2 = state + 2;
        state3 = state + 3;
        c = getc( fp );
    }
    A = A_add( A, state, 1, 1 );
    A = A_close( A );
    fclose( fp );
    return( A );
}

A_OBJECT A_slurp_utf8( char *file, T2_OBJECT T2_Sigma )
{
    A_OBJECT A;
    FILE *fp;
    int i, c1, c2, c3, c4, cp, type;
    char ts[ 5 ];

    if ( file != NULL ) fp = fopen( file, "r" );
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }

    assert( T2_Sigma != NULL );
    assert( T2_Sigma-> T2_int-> Tn_n >= 258 );

    A = A_slurp_octets( file, T2_Sigma );
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
                assert( i + 1 < A-> A_nrows );
                c2 = A-> A_t[ i + 1 ].A_b - 2;
                cp = ( c1 & 0x1f ) + ( c2 & 0x3f );
                if ( ( c2 & 0xc0 ) == 0x80
                  &&   cp > 0x7f ) {
                    ts[ 0 ] = c1;
                    ts[ 1 ] = c2;
                    ts[ 2 ] = '\0';
                }
                A-> A_t[ i ].A_b = T2_insert( T2_Sigma, ts, 2 );
                A-> A_t[ i ].A_c += 1;
                break;

            case 3:
                assert( i + 2 < A-> A_nrows );
                c2 = A-> A_t[ i + 1 ].A_b - 2;
                c3 = A-> A_t[ i + 2 ].A_b - 2;
                cp = ( ( c1 & 0x0f ) << 12 )
                   + ( ( c2 & 0x3f ) <<  6 )
                   +   ( c3 & 0x3f );
                if (    ( c2 & 0xc0 ) == 0x80
                     && ( c3 & 0xc0 ) == 0x80
                     && (    ( 0x03ff < cp && cp < 0xd800 )
                          || ( 0xdfff < cp ) ) ) {
                    ts[ 0 ] = c1;
                    ts[ 1 ] = c2;
                    ts[ 2 ] = c3;
                    ts[ 3 ] = '\0';
                }
                A-> A_t[ i ].A_b = T2_insert( T2_Sigma, ts, 3 );
                A-> A_t[ i ].A_c += 2;
                break;

            case 4:
                assert( i + 3 < A-> A_nrows );
                c2 = A-> A_t[ i + 1 ].A_b - 2;
                c3 = A-> A_t[ i + 2 ].A_b - 2;
                c4 = A-> A_t[ i + 3 ].A_b - 2;
                cp = ( ( c1 & 0x0f ) << 18 )
                   + ( ( c2 & 0x3f ) << 12 )
                   + ( ( c3 & 0x3f ) <<  6 )
                   +   ( c4 & 0x3f );
                if (    ( c2 & 0xc0 ) == 0x80
                     && ( c3 & 0xc0 ) == 0x80
                     && ( c4 & 0xc0 ) == 0x80
                     &&   0xffff < cp && cp <= 0x10ffff ) {
                    ts[ 0 ] = c1;
                    ts[ 1 ] = c2;
                    ts[ 2 ] = c3;
                    ts[ 3 ] = c4;
                    ts[ 4 ] = '\0';
                }
                A-> A_t[ i ].A_b = T2_insert( T2_Sigma, ts, 4 );
                A-> A_t[ i ].A_c += 3;
                break;

            case 0: case 1: case 5:
                break;
        }
    }

    if ( T2_Sigma-> T2_int-> Tn_n > A-> A_nS ) {
        A-> A_nS = T2_Sigma-> T2_int-> Tn_n;
    }
    A = A_min( A );
    fclose( fp );
    return( A );
}

A_OBJECT A_spit_octets( A_OBJECT A, char *file, T2_OBJECT T2_Sigma )
{
    FILE *fp;
    int i, s1, c1;

    if ( file != NULL ) fp = fopen( file, "w" );
    if ( fp == NULL ) {
        Warning( "Can't open file for write" );
        return( NULL );
    }

    assert( T2_Sigma != NULL );
    assert( T2_Sigma-> T2_int-> Tn_n >= 258 );

    A = A_gen_min( A );

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

A_OBJECT A_spit_nibbles( A_OBJECT A, char *file, T2_OBJECT T2_Sigma )
{
    FILE *fp;
    int i, s1;
    int accum = (-1);
    int digit = 0;
    int phase = 0;

    if ( file != NULL ) fp = fopen( file, "w" );
    if ( fp == NULL ) {
        Warning( "Can't open file for write" );
        return( NULL );
    }

    assert( T2_Sigma != NULL );
    assert( T2_Sigma-> T2_int-> Tn_n >= 258 );

    A = A_gen_min( A );

    for ( i = 0; i < A-> A_nrows; ++i ) {
        s1 = A-> A_t[ i ].A_b;

        switch ( s1 ) {
        case 50: case 51: case 52: case 53: case 54:
        case 55: case 56: case 57: case 58: case 59:
            digit = s1 - 50;
            break;
        case 67: case 68: case 69: case 70: case 71: case 72:
            digit = s1 - 57;
            break;
        case 97:
            digit = -1;
            break;
        case 1:
            digit = -2;
            break;
        default:
            printf( "%d\n", s1 );
            Error( "Illegal character" );
        }

        if ( digit == -2 ) { break; }

        switch ( phase ) {
        case 0:
            assert( digit >= 0 );
            accum = digit * 16;
            phase = 1;
            break;
        case 1:
            assert( digit >= 0 );
            accum += digit;
            phase = 2;
            break;
        case 2:
            assert( digit == -1 );
            fputc( accum, fp );
            accum = (-1);
            phase = 0;
            break;
        }
    }
    assert( accum == -1 );
    assert( digit == -2 );
    assert( phase == 0 );
    fclose( fp );
    return( A );
}

A_OBJECT A_spit_utf8( A_OBJECT A, char *file, T2_OBJECT T2_Sigma )
{
    FILE *fp;
    int i, s1, c1, type;
    int j, length;
    char *cstr;

    if ( file != NULL ) fp = fopen( file, "w" );
    if ( fp == NULL ) {
        Warning( "Can't open file for write" );
        return( NULL );
    }

    assert( T2_Sigma != NULL );
    assert( T2_Sigma-> T2_int-> Tn_n >= 258 );

    A = A_gen_min( A );

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
            cstr = T2_name( T2_Sigma, s1 );
            length = T2_length( T2_Sigma, s1 );
            for ( j = 0; j < length; ++j ) {
                fputc( cstr[ j ], fp );
            }
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
