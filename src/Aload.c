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

#include "O.h"

#define MAX_TOKEN       1024

static FILE *fp;
FILE *fpin, *fpout;
static int c;

char *get_name()
{
    int i;
    static char token[ MAX_TOKEN ];

    while ( c == ' ' || c == '\t' ) c = getc( fp );
    i = 0;
    while ( c != ' ' && c != '\t' && c != '\n' ) {
        if ( c == EOF ) {
            Warning( "get_name: Unexpected EOF" );
            return( NULL );
        }
        if ( c == '\\' ) {
            c = getc( fp );
            switch( c ) {
            case 'n':
                c = '\n';
                break;
            case 't':
                c = '\t';
                break;
            case '_':
                c =  ' ';
                break;
            }
        }
        token[ i++ ] = c;
        c = getc( fp );
        if ( i >= MAX_TOKEN ) {
            Warning( "get_name: Name too long" );
            return( NULL );
        }
    }
    if ( i == 0 ) {
        Warning( "get_name: Zero length name" );
        return( NULL );
    }
    token[ i ] = 0;
    return( token );
}

int get_nl()
{
    while ( c == ' ' || c == '\t' ) c = getc( fp );
    if ( c != '\n' ) {
        Warning( "get_nl: New line expected" );
        return( 0 );
    }
    c = getc( fp );
    return( 1 );
}

A_OBJECT A_load( char *file, T2_OBJECT T2_Sigma )
{
    int from, symb, to, tape, ntapes, i;
    A_OBJECT A;
    char *t;
    A_row *p;

    if ( file != NULL )             fp = fopen( file, FOPEN_READ );
    else if ( fpin != NULL )        fp = fpin;
    else                            fp = stdin;
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }
    A = A_create();
    if ( T2_Sigma == NULL
            || T2_insert( T2_Sigma, "^^", 2 ) != 0
            || T2_insert( T2_Sigma, "-|", 2 ) != 1 ) Error( "A_load: BOTCH 1" );
    A-> A_nT = ntapes = 1;
    c = getc( fp );
    if ( c == 'I' ) {
        A_destroy( A );
        fclose( fp );
        return( A_load_save( file, T2_Sigma ) );
    } else if ( c >= 0 ) {
        A_destroy( A );
        fclose( fp );
        return( A_load_pr( file, T2_Sigma ) );
    } else {
        A-> A_nT = ntapes = c + 1;
        c = getc(fp);
        if ( !get_nl() ) {
            A_destroy( A );
            return( NULL );
        }
        while ( c != EOF ) {
            from = (c & 0377) * 256;
            c = getc(fp);
            from += c & 0377;
            c = getc(fp);
            to = (c & 0377) * 256;
            c = getc(fp);
            to += c & 0377;
            c = getc(fp);
            if ( (t = get_name()) == NULL ) {
                A_destroy( A );
                return( NULL );
            }
            if ( t[1] == '.' && t[0] >= '0' && t[0] <= '9' ) {
                tape = t[0] - '0';
                if ( tape >= ntapes ) {
                    for( p = A-> A_t + A-> A_nrows;
                            --p >= A-> A_t; ) {
                        i = p-> A_b;
                        if ( i > 1 )
                            i = i / ntapes * (tape+1)
                                + i % ntapes;
                        p-> A_b = i;
                    }
                    A-> A_nT = ntapes = tape + 1;
                }
                symb = T2_insert( T2_Sigma, t + 2, strlen( t + 2 ) );
                if ( symb == 1 && ntapes > 1 ) A-> A_ems = 1;
                symb = symb * ntapes + tape;
            } else {
                symb = T2_insert( T2_Sigma, t, strlen( t ) );
                if ( symb == 1 && ntapes > 1 ) A-> A_ems = 1;
                if ( symb != 1 ) symb *= ntapes;
            }
            A = A_add( A, from, symb, to );
            if ( !get_nl() ) {
                A_destroy( A );
                return( NULL );
            }
        }
        if ( file != NULL ) fclose( fp );
        A = A_close( A );
        A-> A_mode = DFA_MIN;
        return( A );
    }
}

A_OBJECT A_lwds( char *file, T2_OBJECT T2_Sigma )
{
    A_OBJECT A, As;
    char *p;
    int i, nQ;

    if ( file != NULL )             fp = fopen( file, FOPEN_READ );
    else if ( fpin != NULL )        fp = fpin;
    else                            fp = stdin;
    if ( fp == NULL ) {
        Warning( "File does not exist" );
        return( NULL );
    }

    assert( T2_Sigma != NULL );
    assert( T2_Sigma-> T2_int-> Tn_n >= 258 );

    A = A_create();
    As = A_create();

    c = getc( fp );
    while ( c != EOF ) {
        if ( A-> A_nQ > 64000 || A-> A_nrows > 100000 ) {
            fprintf( fpout, "Pause to compress ...\n" );
            A = A_close( A );
            A-> A_mode = NFA_CLOSED;
            As = A_union( A_min( A ), A_min( As ) );
            As = A_close( As );
            As-> A_mode = NFA_CLOSED;
            A = A_create();
            fprintf( fpout,
                     "Done Compress (states %d transitions %d)\n",
                     As-> A_nQ, As-> A_nrows );
        }
        nQ = A-> A_nQ;
        if ( (p = get_name()) == NULL ) {
            A_destroy( A );
            A_destroy( As );
            return( NULL );
        }
        for( i = 0; p[i] != 0; ++i ) {
            A = A_add( A, (i == 0) ? 0 : nQ + i - 1,
                ( p[ i ] & 0xff ) + 2, nQ + i );
        }
        A = A_add( A, nQ + i - 1, 1, 1 );
        if ( !get_nl() ) {
            A_destroy( A );
            A_destroy( As );
            return( NULL );
        }
    }
    if ( file != NULL ) fclose( fp );
    A = A_close( A );
    A-> A_mode = NFA_CLOSED;
    As = A_union( A_min( A ), A_min( As ) );
    As = A_close( As );
    As-> A_mode = NFA_CLOSED;
    return( As );
}

A_OBJECT A_prsseq( A_OBJECT A, char *file, T2_OBJECT T2_Sigma )
{
    int t;
    A_row *p, *pz;
    int i, n_read, n_write;
    int ss_states = 0;

    if ( A == NULL || T2_Sigma == NULL ) return( A );
    if ( A-> A_nT != 2 ) Error( "A_prsseq: Not two tapes" );
    if ( A-> A_mode < SSEQ ) A = A_min( A_sseq( A ) );
    if ( file != NULL ) {
        if ( strcmp( file, "devnull" ) == 0 ) return( A );
        else fp = fopen( file, FOPEN_WRITE );
    } else {
        fp = fpout;
        if ( fp == NULL ) fp = stdin;
    }
    if ( fp == NULL ) {
        Warning( "Cannot open file" );
        return( A );
    }
    for( i = 0; i < A-> A_nQ; ++i ) {
        n_read = 0;
        n_write = 0;
        pz = A-> A_p[i+1];
        for( p = A-> A_p[i]; p < pz; p++ ) {
            if ( p-> A_b % 2 == 0 ) ++n_read;
            else ++n_write;
        }
        if ( ( n_read > 0 && n_write > 0 )
                || n_write > 1 )
            Error( "A_prsseq: Not Subsequential" );
        if ( i == START || n_write == 0 ) {
            ++ss_states;
            pz = A-> A_p[i+1];
            for( p = A-> A_p[i]; p < pz; p++ ) {

                if ( ( t = p-> A_a ) == START ) fprintf( fp, "(START)  " );
                else if ( t == FINAL )          fprintf( fp, "(FINAL)  " );
                else                            fprintf( fp, "%-7d  ", t );
                if ( n_write == 0 ) {
                    fprintf( fp, "%s", T2_name_pr( T2_Sigma, p-> A_b / 2 ) );
                    t = p-> A_c;
                    fprintf( fp, " [" );
                } else {
                    t = p-> A_a;
                    fprintf( fp, "[" );
                }
                while ( A-> A_p[t] + 1 == A-> A_p[t+1]
                        && A-> A_p[t]-> A_b % 2 == 1 ) {
                    if ( A-> A_p[t]-> A_b != 1 ) {
                        fprintf( fp, " %s",
                            T2_name_pr( T2_Sigma, A-> A_p[t]-> A_b / 2 ) );
                    }
                    t = A-> A_p[t]-> A_c;
                }
                fprintf( fp, " ]  \t" );
                if ( t == START )               fprintf( fp, "(START)\n" );
                else if ( t == FINAL )          fprintf( fp, "(FINAL)\n" );
                else                            fprintf( fp, "%d\n", t );

            }
        }
    }
    fprintf( fp, "\n%d states in subsequential transducer\n", ss_states );
    if ( fp != stdout )
        printf( "%d states in subsequential transducer\n", ss_states );
    if ( file != NULL ) {
        fclose( fp );
    } else  if ( fflush( stdout ) == EOF ) Error( "A_store: fflush" );
    return( A );
}
