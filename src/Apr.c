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

A_OBJECT A_pr( A_OBJECT A, char *file, T2_OBJECT T2_Sigma )
{
    int t;
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
    pz = A-> A_t + A-> A_nrows;
    for( p = A-> A_t; p < pz; p++ ) {
        if ( ( t = p-> A_a ) == START ) fprintf( fp, "(START) " );
        else if ( t == FINAL )          fprintf( fp, "(FINAL) " );
        else                            fprintf( fp, "%d ", t );
        if ( ( t = p-> A_b ) <= 1 || A-> A_nT == 1 ) {
            fprintf( fp, "%s", T2_name_pr( T2_Sigma, t ) );
        } else {
            fprintf( fp, "%1d.%s", t % A-> A_nT,
                T2_name_pr( T2_Sigma, t / A-> A_nT ) );
        }
        if ( ( t = p-> A_c ) == START ) fprintf( fp, " (START)\n" );
        else if ( t == FINAL )          fprintf( fp, " (FINAL)\n" );
        else                            fprintf( fp, " %d\n", t );
    }
    if ( file != NULL ) {
        fclose( fp );
    } else  if ( fflush( stdout ) == EOF ) Error( "A_pr: fflush" );
    return( A );
}

A_OBJECT A_load_pr( char *file, T2_OBJECT T2_Sigma )
{
    int from_state, to_state, tape_number, ntapes, l;
    int c, d, j, transition_label;
    A_OBJECT A;
    Tn_OBJECT TQ;
    char *buffer;
    A_row *p;
    FILE *fp;

    buffer = Salloc( 100 );

    if ( file != NULL )             fp = fopen( file, "r" );
    else if ( fpin != NULL )        fp = fpin;
    else                            fp = stdin;
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }
    A = A_create();
    A-> A_nT = ntapes = 1;

    if ( T2_Sigma == NULL
            || T2_insert( T2_Sigma, "^^", 2 ) != 0
            || T2_insert( T2_Sigma, "-|", 2 ) != 1 )
                Error( "A_load: BOTCH 1" );

    TQ = Tn_create();
    if ( Tn_insert( TQ, "(START)", 7 ) != START
      || Tn_insert( TQ, "(FINAL)", 7 ) != FINAL )
        Error( "A_load: BOTCH 2" );

    c = getc( fp );
    assert( Ssize( buffer ) >= 100 );

NEXT_ROW:

/* From state */
/* Here c should contain the first character of a non empty line. */

    if ( c == ' ' || c == '\t' || c == '\n' || c == EOF ) { fail(1); }

    l = 0;
    if ( l >= Ssize( buffer) ) { buffer = Srealloc( buffer, 2 * l ); }
    buffer[ l++ ] = c; c = getc( fp );

    while ( c != ' ' && c != '\t' && c != '\n' && c != EOF ) {
        if ( l >= Ssize( buffer ) ) { buffer = Srealloc( buffer, 2 * l ); }
        buffer[ l++ ] = c; c = getc( fp );
    }

    buffer[ l ] = '\0';
    from_state = Tn_insert( TQ, buffer, l );

/* Transition label */
/* Here c should contain a tab or blank separator. */
/* If c is new line or EOF, this is an error. */

    if ( c == '\n' || c == EOF ) { fail(2); }

    assert( c == ' ' || c == '\t' );  c = getc( fp );

    l = 0;
    tape_number = 0;

/* Look for a leading tape number. */

    while ( c >= '0' && c <= '9' ) {
        tape_number = tape_number * 10 + ( c - '0' );
        if ( l >= Ssize( buffer ) ) { buffer = Srealloc( buffer, 2 * l ); }
        buffer[ l++ ] = c; c = getc( fp );
    }

    if ( l > 0 && c == '.' ) {
        l = 0;
        c = getc( fp );
    } else {
        tape_number = (-1);
    }

    if ( tape_number >= ntapes ) {
        for ( p = A-> A_t + A-> A_nrows; --p >= A-> A_t; ) {
            j = p-> A_b;
            if ( j > 1 ) {
                j = j / ntapes * ( tape_number + 1 ) + j % ntapes;
            }
            p-> A_b = j;
        }
        A-> A_nT = ntapes = tape_number + 1;
    }

/* Tape number processed. */
/* Now c must be a non-delimiter. */

    if ( c == ' ' || c == '\t' || c == '\n' || c == EOF ) { fail(3); }

    while ( c != ' ' && c != '\t' && c != '\n' && c != EOF ) {
        if ( l >= Ssize( buffer ) ) { buffer = Srealloc( buffer, 2 * l ); }
        if ( c == '\\' ) {
            c = getc( fp );
            if ( c == '_' ) { c = ' '; }
            else if ( c == 't' ) { c = '\t'; }
            else if ( c == 'n' ) { c = '\n'; }
            else if ( c == 'r' ) { c = '\r'; }
            else if ( c == 'x' ) {
                c = getc( fp );
                if ( c >= '0' && c <= '9' ) { d = c - '0'; }
                else if ( c >= 'A' && c <= 'F' ) { d = c + 10 - 'A'; }
                else if ( c >= 'a' && c <= 'f' ) { d = c + 10 - 'a'; }
                else { fail(4); }
                d *= 16;
                c = getc( fp );
                if ( c >= '0' && c <= '9' ) { d += c - '0'; }
                else if ( c >= 'A' && c <= 'F' ) { d += c + 10 - 'A'; }
                else if ( c >= 'a' && c <= 'f' ) { d += c + 10 - 'a'; }
                else { fail(5); }
                c = d;
            } else if ( c != EOF ) { fail(6); }
        }

        buffer[ l++ ] = c; c = getc( fp );
    }

    buffer[ l ] = '\0';
    transition_label = T2_insert( T2_Sigma, buffer, l );

/* To state */
/* Here c must be a delimiter. */

    if ( c != ' ' && c != '\t' && c != '\n' && c != EOF ) { fail(7); }
    c = getc( fp );

    l = 0;
    while ( c != ' ' && c != '\t' && c != '\n' && c != EOF ) {
        if ( l >= Ssize( buffer ) ) { buffer = Srealloc( buffer, 2 * l ); }
        buffer[ l++ ] = c; c = getc( fp );
    }

    buffer[ l ] = '\0';
    to_state = Tn_insert( TQ, buffer, l );

/* Process transition */
/* Here c must be the end of a line or EOF. */

    if ( c != '\n' && c != EOF ) { fail(8); } c = getc( fp );

    if ( tape_number >= 0 ) {
        A = A_add( A, from_state,
            transition_label * ntapes + tape_number, to_state );
    } else {
        A = A_add( A, from_state, transition_label, to_state );
    }

    if ( c != EOF ) { goto NEXT_ROW; }

/* Done: package up result */

    Sfree( buffer );
    if ( file != NULL ) fclose( fp );
    Tn_destroy( TQ );
    return ( A );
/*  return( A_close( A_rename( A, (SHORT *) NULL ) ) ); */

FAIL_FORMAT:

    fprintf( fpout, "A_load_pr: Error code = %d\n", error_code );
    Error( "Illegal pr format" );
    Sfree( buffer );
    if ( file != NULL ) fclose( fp );
    A_destroy( A );
    Tn_destroy( TQ );
    return ( 0 );
}
