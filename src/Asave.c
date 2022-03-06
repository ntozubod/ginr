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

A_OBJECT A_save( A_OBJECT A, char *file, Tn_OBJECT Tn_Sigma )
{
    int t, tape_number, label_length, tt, i;
    char *label_name;
    A_row *p, *pz;
    FILE *fp;

    if ( A == NULL || Tn_Sigma == NULL ) return( A );
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
    fprintf( fp, "INR210\t%d\t%d\n", A-> A_nT, A-> A_nrows );
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
            label_length = Tn_length( Tn_Sigma, t );
            label_name = Tn_name( Tn_Sigma, t );
        } else {
            tape_number = t % A-> A_nT;
            tt = t / A-> A_nT;
            if ( tt == 1 ) {
                label_length = 0;
                label_name = "";
            } else {
                label_length = Tn_length( Tn_Sigma, tt );
                label_name = Tn_name( Tn_Sigma, tt );
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

A_OBJECT A_load_save( char *file, Tn_OBJECT Tn_Sigma )
{
    int c, number_tapes, number_rows;
    int from_state, to_state, tape_no, length, label;
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
    if ( Tn_Sigma == NULL
        || Tn_insert( Tn_Sigma, "^^", 2 ) != 0
        || Tn_insert( Tn_Sigma, "-|", 2 ) != 1 )
            Error( "A_load: BOTCH 1" );

    c = getc( fp );

/* Magic Number */

    if ( c != 'I' ) { goto FAIL_FORMAT; }  c = getc( fp );
    if ( c != 'N' ) { goto FAIL_FORMAT; }  c = getc( fp );
    if ( c != 'R' ) { goto FAIL_FORMAT; }  c = getc( fp );
    if ( c != '2' ) { goto FAIL_FORMAT; }  c = getc( fp );
    if ( c != '1' ) { goto FAIL_FORMAT; }  c = getc( fp );
    if ( c != '0' ) { goto FAIL_FORMAT; }  c = getc( fp );
    if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );

/* Number of tapes */

    if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        number_tapes = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        number_tapes = number_tapes * 10  +  ( c - '0' );
            c = getc( fp );
        if ( number_tapes >= MAXSHORT ) { goto FAIL_FORMAT; }
    }
    A-> A_nT = number_tapes;
    if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );

/* Number of rows */

    if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        number_rows = c - '0'; c = getc( fp );

    while ( c != '\n' ) {
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        number_rows = number_rows * 10  +  ( c - '0' );
            c = getc( fp );
        if ( number_rows >= MAXSHORT ) { goto FAIL_FORMAT; }
    }
    if ( c != '\n' ) { goto FAIL_FORMAT; } c = getc( fp );

/* Get a row */

NEXT_ROW:

/* From state */

    if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        from_state = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        from_state = from_state * 10  +  ( c - '0' );
            c = getc( fp );
        if ( from_state >= MAXSHORT ) { goto FAIL_FORMAT; }
    }
    if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );

/* To state */

    if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        to_state = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        to_state = to_state * 10  +  ( c - '0' );
            c = getc( fp );
        if ( to_state >= MAXSHORT ) { goto FAIL_FORMAT; }
    }
    if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );

/* Tape number */

    if ( c == '-' ) {
        c = getc( fp );
        if ( c != '1' ) { goto FAIL_FORMAT; } c = getc( fp );
        tape_no = (-1);
    } else {
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            tape_no = c - '0'; c = getc( fp );

        while ( c != '\t' ) {
            if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            tape_no = tape_no * 10  +  ( c - '0' );
                c = getc( fp );
            if ( tape_no >= MAXSHORT ) { goto FAIL_FORMAT; }
        }
    }
    if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );

/* Token length */

    if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        length = c - '0'; c = getc( fp );

    while ( c != '\t' ) {
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
        length = length * 10  +  ( c - '0' );
            c = getc( fp );
        if ( length >= MAXSHORT ) { goto FAIL_FORMAT; }
    }
    if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );

/* Get a token */

    if ( length + 1 >= Ssize( buffer ) ) {
        buffer = Srealloc( buffer, length + 1 );
    }

    i = 0;
    while ( i < length ) {
        buffer[ i++ ] = c;
        c = getc( fp );
        if ( c == EOF ) { goto FAIL_FORMAT; }
    }
    buffer[ i ] = '\0';
    if ( c != '\n' ) { goto FAIL_FORMAT; } c = getc( fp );

/* Process a line here */

    if ( tape_no == -1 ) {
        A = A_add( A, from_state, 0, to_state );
        if ( length != 0 ) { goto FAIL_FORMAT; }
    } else if ( length == 0 ) {
        label = 1 * number_tapes + tape_no;
        A = A_add( A, from_state, label, to_state );
    } else {
        index = Tn_insert( Tn_Sigma, buffer, length );
        label = index * number_tapes + tape_no;
        A = A_add( A, from_state, label, to_state );
    }

    if ( c != EOF ) { goto NEXT_ROW; }

/* Done: package up result */

    Sfree( buffer );
    return ( A );

FAIL_FORMAT:

    Sfree( buffer );
    A_destroy( A );
    return ( 0 );
}
