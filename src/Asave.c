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

static int error_code = 0;
#define fail(x) { error_code = x; goto FAIL_FORMAT; }

A_OBJECT A_save( A_OBJECT A, char *file, T2_OBJECT T2_Sigma )
{
    int t, tape_number, label_length, tt, i;
    char *label_name;
    A_row *p, *pz;
    FILE *fp;

    if ( A == NULL || T2_Sigma == NULL ) return( A );
    if ( file != NULL ) {
        if ( strcmp( file, "devnull" ) == 0 ) return( A );
        else fp = fopen( file, "w" );
    } else {
        fp = fpout;
        if ( fp == NULL ) fp = stdin;
    }
    if ( fp == NULL ) {
        Warning( "Cannot open file" );
        return( A );
    }
    fprintf( fp, "INR210\t%d\t%d\t%d\t%d\n",
        A-> A_nT, A-> A_nrows, A-> A_nQ, A-> A_nS );
    pz = A-> A_t + A-> A_nrows;
    for( p = A-> A_t; p < pz; p++ ) {
        t = p-> A_b;
        if ( t == 0 ) {
            tape_number = -1;
            label_length = 0;
            label_name = "";
        } else if ( t == 1 ) {
            tape_number = 0;
            label_length = 0;
            label_name = "";
        } else if ( A-> A_nT == 1 ) {
            tape_number = 0;
            label_length = T2_length( T2_Sigma, t );
            label_name = T2_name( T2_Sigma, t );
        } else {
            tape_number = t % A-> A_nT;
            tt = t / A-> A_nT;
            if ( tt == 1 ) {
                label_length = 0;
                label_name = "";
            } else {
                label_length = T2_length( T2_Sigma, tt );
                label_name = T2_name( T2_Sigma, tt );
            }
        }
        fprintf( fp, "%d\t%d\t%d\t%d\t",
            p-> A_a, p-> A_c, tape_number, label_length );
        for( i = 0; i < label_length; ++i ) {
            (void) putc( label_name[ i ], fp );
        }
        (void) putc( '\n', fp );
    }
    if ( file != NULL ) {
        fclose( fp );
    } else  if ( fflush( stdout ) == EOF ) Error( "A_save: fflush" );
    return( A );
}

A_OBJECT A_load_save( char *file, T2_OBJECT T2_Sigma )
{
    int c, number_tapes, number_rows, number_states, number_symbols;
    int from_state, to_state, row_no, tape_no, length, label;
    int i, index;
    char *buffer;
    FILE *fp;
    A_OBJECT A;

    buffer = Salloc( 100 );

    if ( file != NULL )             fp = fopen( file, "r" );
    else if ( fpin != NULL )        fp = fpin;
    else                            fp = stdin;
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }
    A = A_create();
    if ( T2_Sigma == NULL
        || T2_insert( T2_Sigma, "^^", 2 ) != 0
        || T2_insert( T2_Sigma, "-|", 2 ) != 1 )
            Error( "A_load: BOTCH 1" );

    c = getc( fp );

/* Magic Number */

    if ( c != 'I' ) { fail(1); }  c = getc( fp );
    if ( c != 'N' ) { fail(2); }  c = getc( fp );
    if ( c != 'R' ) { fail(3); }  c = getc( fp );
    if ( c != '2' ) { fail(4); }  c = getc( fp );
    if ( c != '1' ) { fail(5); }  c = getc( fp );
    if ( c != '0' ) { fail(6); }  c = getc( fp );
    if ( c != '\t' ) { fail(7); } c = getc( fp );

/* Number of tapes */

    if ( c < '0' || c > '9' ) { fail(8); }
        number_tapes = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { fail(9); }
        number_tapes = number_tapes * 10  +  ( c - '0' );
            c = getc( fp );
        if ( number_tapes >= MAXSHORT ) { fail(10); }
    }
    A-> A_nT = number_tapes;
    if ( c != '\t' ) { fail(11); } c = getc( fp );

/* Number of rows */

    if ( c < '0' || c > '9' ) { fail(12); }
        number_rows = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { fail(13); }
        number_rows = number_rows * 10  +  ( c - '0' );
            c = getc( fp );
        if ( number_rows >= MAXSHORT ) { fail(14); }
    }
    if ( c != '\t' ) { fail(15); } c = getc( fp );

/* Number of states */

    if ( c < '0' || c > '9' ) { fail(16); }
        number_states = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { fail(17); }
        number_states = number_states * 10  +  ( c - '0' );
            c = getc( fp );
        if ( number_states >= MAXSHORT ) { fail(18); }
    }
    if ( c != '\t' ) { fail(19); } c = getc( fp );

/* Bound on number of letters in alphabet */

    if ( c < '0' || c > '9' ) { fail(20); }
        number_symbols = c - '0'; c = getc( fp );

    while ( c != '\n' ) {
        if ( c < '0' || c > '9' ) { fail(21); }
        number_symbols = number_symbols * 10  +  ( c - '0' );
            c = getc( fp );
        if ( number_symbols >= MAXSHORT ) { fail(22); }
    }
    if ( c != '\n' ) { fail(23); } c = getc( fp );

    row_no = -1;

NEXT_ROW:

    if ( ++row_no >= number_rows ) { fail(24); }

/* From state */

    if ( c < '0' || c > '9' ) { fail(25); }
        from_state = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { fail(26); }
        from_state = from_state * 10  +  ( c - '0' );
            c = getc( fp );
        if ( from_state >= MAXSHORT ) { fail(27); }
    }
    if ( c != '\t' ) { fail(28); } c = getc( fp );
    if ( from_state >= number_states ) { fail(29); }

/* To state */

    if ( c < '0' || c > '9' ) { fail(30); }
        to_state = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { fail(31); }
        to_state = to_state * 10  +  ( c - '0' );
            c = getc( fp );
        if ( to_state >= MAXSHORT ) { fail(32); }
    }
    if ( c != '\t' ) { fail(33); } c = getc( fp );
    if ( to_state >= number_states ) { fail(34); }

/* Tape number */

    if ( c == '-' ) {
        c = getc( fp );
        if ( c != '1' ) { fail(35); } c = getc( fp );
        tape_no = (-1);
    } else {
        if ( c < '0' || c > '9' ) { fail(36); }
            tape_no = c - '0'; c = getc( fp );

        while ( c != '\t' ) {
            if ( c < '0' || c > '9' ) { fail(37); }
            tape_no = tape_no * 10  +  ( c - '0' );
                c = getc( fp );
            if ( tape_no >= MAXSHORT ) { fail(38); }
        }
    }
    if ( c != '\t' ) { fail(39); } c = getc( fp );
    if ( tape_no >= number_tapes ) { fail(40); }

/* Token length */

    if ( c < '0' || c > '9' ) { fail(41); }
        length = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { fail(42); }
        length = length * 10  +  ( c - '0' );
            c = getc( fp );
        if ( length >= MAXSHORT ) { fail(43); }
    }
    if ( c != '\t' ) { fail(44); } c = getc( fp );

/* Get a token */

    if ( length + 1 >= Ssize( buffer ) ) {
        buffer = Srealloc( buffer, length + 1 );
    }

    i = 0;
    while ( i < length ) {
        buffer[ i++ ] = c;
        c = getc( fp );
        if ( c == EOF ) { fail(45); }
    }
    buffer[ i ] = '\0';
    if ( c != '\n' ) { fail(46); } c = getc( fp );

/* Process a line here */

    if ( tape_no == -1 ) {
        A = A_add( A, from_state, 0, to_state );
        if ( length != 0 ) { fail(47); }
    } else if ( length == 0 ) {
        if ( to_state == 1 ) {
            label = 1;
        } else {
            label = 1 * number_tapes + tape_no;
        }
        A = A_add( A, from_state, label, to_state );
    } else {
        index = T2_insert( T2_Sigma, buffer, length );
        if ( index >= number_symbols ) { fail(48); }
        label = index * number_tapes + tape_no;
        A = A_add( A, from_state, label, to_state );
    }

    if ( c != EOF ) { goto NEXT_ROW; }

/* Done: package up result */

    Sfree( buffer );
    return ( A );

FAIL_FORMAT:

    fprintf( fpout, "A_load_save: Error code = %d\n", error_code );
    Error( "Illegal save format" );
    Sfree( buffer );
    A_destroy( A );
    return ( 0 );
}
