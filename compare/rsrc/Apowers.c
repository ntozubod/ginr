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

A_OBJECT A_catpow( A_OBJECT A, int n )
{
    A_OBJECT Aprod;
    if ( n < 0 ) Error( "A_catpow: negative power" );
    A = A_min( A );
    if ( n % 2 )    Aprod = A_copy( A );
    else            Aprod = A_lambda();
    n /= 2;
    while( n > 0 ) {
        A = A_min( A_concat( A, A_copy( A ) ) );
        if ( n % 2 ) Aprod = A_min( A_concat( Aprod, A_copy( A ) ) );
        n /= 2;
    }
    A_destroy( A );
    return( Aprod );
}

A_OBJECT A_ident( A_OBJECT A )
{
    A_OBJECT A2;
    int i, sigma, last;
    A = A_min( A_alph( A ) );
    A2 = A_create();
    A2-> A_nT = 2;
    last = A-> A_nrows + 1;
    for( i = 0; i < A-> A_nrows - 1; i++ ) {
        sigma = A-> A_t[i].A_b;
        A2 = A_add( A2, 0, sigma * 2, i + 2 );
        A2 = A_add( A2, i + 2, sigma * 2 + 1, last );
    }
    A2 = A_add( A2, last, 1, 1 );
    A_destroy( A );
    return( A_star( A2 ) );
}

A_OBJECT A_cmpow( A_OBJECT A, int n )
{
    A_OBJECT Aprod;
    if ( n < 0 ) Error( "A_cmpow: negative power" );
    A = A_min( A );
    if ( n % 2 )    Aprod = A_copy( A );
    else            Aprod = A_ident( A_copy( A ) );
    n /= 2;
    while( n > 0 ) {
        A = A_min( A_compose( A, A_copy( A ) ) );
        if ( n % 2 ) Aprod = A_min( A_compose( Aprod, A_copy( A ) ) );
        n /= 2;
    }
    A_destroy( A );
    return( Aprod );
}
