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

int A_valid_utf8_at( char *s, int i, int l, int *cp_ref ) {
    int c1 = s[ i ];
    int type, c2, c3, c4, cp;

    *cp_ref = -1;

         if ( ( c1 & 0x80 ) == 0x00 ) { type = 0; }
    else if ( ( c1 & 0xc0 ) == 0x80 ) { type = 1; }
    else if ( ( c1 & 0xe0 ) == 0xc0 ) { type = 2; }
    else if ( ( c1 & 0xf0 ) == 0xe0 ) { type = 3; }
    else if ( ( c1 & 0xf8 ) == 0xf0 ) { type = 4; }
    else { type = 5; }

    switch ( type ) {

    case 0:
        cp = c1;
        *cp_ref = cp;
        return( 1 );

    case 2:
        if ( i + 1 >= l ) { return( 0 ); }
        c2 = s[ i + 1 ];
        cp = ( ( c1 & 0x1f ) << 6 )
           +   ( c2 & 0x3f );
        if ( ( c2 & 0xc0 ) == 0x80
          &&   cp > 0x7f ) {
            *cp_ref = cp;
            return( 2 );
        }
        break;

    case 3:
        if ( i + 2 >= l ) { return( 0 ); }
        c2 = s[ i + 1 ];
        c3 = s[ i + 2 ];
        cp = ( ( c1 & 0x0f ) << 12 )
           + ( ( c2 & 0x3f ) <<  6 )
           +   ( c3 & 0x3f );
        if (    ( c2 & 0xc0 ) == 0x80
             && ( c3 & 0xc0 ) == 0x80
             && (    ( 0x03ff < cp && cp < 0xd800 )
                  || ( 0xdfff < cp ) ) ) {
            *cp_ref = cp;
            return( 3 );
        }
        break;

    case 4:
        if ( i + 3 >= l ) { return( 0 ); }
        c2 = s[ i + 1 ];
        c3 = s[ i + 2 ];
        c4 = s[ i + 3 ];
        cp = ( ( c1 & 0x0f ) << 18 )
           + ( ( c2 & 0x3f ) << 12 )
           + ( ( c3 & 0x3f ) <<  6 )
           +   ( c4 & 0x3f );
        if (    ( c2 & 0xc0 ) == 0x80
             && ( c3 & 0xc0 ) == 0x80
             && ( c4 & 0xc0 ) == 0x80
             &&   0xffff < cp && cp <= 0x10ffff ) {
            *cp_ref = cp;
            return( 4 );
        }
        break;

    case 1: case 5:
        break;
    }
    return( 0 );
}

int A_unicode_printable( int cp ) {
/* Exclude all control, spacing, and non-characters */
    if ( (  0x0000 <= cp && cp <    0x0021 )
      || (  0x007F <= cp && cp <    0x00A1 )
      || (  0x00AD <= cp && cp <    0x00AE )
      || (  0x0600 <= cp && cp <    0x0606 )
      || (  0x061C <= cp && cp <    0x061D )
      || (  0x06DD <= cp && cp <    0x06DE )
      || (  0x070F <= cp && cp <    0x0710 )
      || (  0x0890 <= cp && cp <    0x0892 )
      || (  0x08E2 <= cp && cp <    0x08E3 )
      || (  0x1680 <= cp && cp <    0x1681 )
      || (  0x180E <= cp && cp <    0x180F )
      || (  0x2000 <= cp && cp <    0x2010 )
      || (  0x2028 <= cp && cp <    0x2030 )
      || (  0x205F <= cp && cp <    0x2065 )
      || (  0x2066 <= cp && cp <    0x2070 )
      || (  0x3000 <= cp && cp <    0x3001 )
      || (  0xD800 <= cp && cp <    0xF900 )
      || (  0xFDD0 <= cp && cp <    0xFDF0 )
      || (  0xFEFF <= cp && cp <    0xFF00 )
      || (  0xFFF9 <= cp && cp <    0xFFFC )
      || (  0xFFFE <= cp && cp <   0x10000 )
      || ( 0x110BD <= cp && cp <   0x110BE )
      || ( 0x110CD <= cp && cp <   0x110CE )
      || ( 0x13430 <= cp && cp <   0x13439 )
      || ( 0x1BCA0 <= cp && cp <   0x1BCA4 )
      || ( 0x1D173 <= cp && cp <   0x1D17B )
      || ( 0x1FFFE <= cp && cp <   0x20000 )
      || ( 0x2FFFE <= cp && cp <   0x30000 )
      || ( 0x3FFFE <= cp && cp <   0x40000 )
      || ( 0x4FFFE <= cp && cp <   0x50000 )
      || ( 0x5FFFE <= cp && cp <   0x60000 )
      || ( 0x6FFFE <= cp && cp <   0x70000 )
      || ( 0x7FFFE <= cp && cp <   0x80000 )
      || ( 0x8FFFE <= cp && cp <   0x90000 )
      || ( 0x9FFFE <= cp && cp <   0xA0000 )
      || ( 0xAFFFE <= cp && cp <   0xB0000 )
      || ( 0xBFFFE <= cp && cp <   0xC0000 )
      || ( 0xCFFFE <= cp && cp <   0xD0000 )
      || ( 0xDFFFE <= cp && cp <   0xE0000 )
      || ( 0xE0001 <= cp && cp <   0xE0002 )
      || ( 0xE0020 <= cp && cp <   0xE0080 )
      || ( 0xEFFFE <= cp && cp <= 0x10FFFF ) ) {
        return( 0 );
    }
    return( 1 );
}
