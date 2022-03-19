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

Q_OBJECT Q_create( int tapeno, int length, char *cstr )
{
    Q_OBJECT Q;
    Q = (Q_OBJECT) Salloc( sizeof(struct Q_desc) );
    Q-> Type = Q_Object;
    Q-> Q_tapeno = tapeno;
    Q-> Q_length = length;
    Q-> Q_cstr = Salloc( length + 1 );
    copymem( length, cstr, Q-> Q_cstr );
    Q-> Q_cstr[ length ] = '\0';
    return( Q );
}

Q_OBJECT Q_fromP( P_OBJECT P )
{
    Q_OBJECT Q;
    int i = 0;
    int l = P-> P_length;
    char *s = P-> P_cstr;
    int c = s[ i++ ];
    int tapeno = 0;

    Q = (Q_OBJECT) Salloc( sizeof(struct Q_desc) );
    Q-> Type = Q_Object;

    while ( i <= l && c >= '0' && c <= '9' ) {
        tapeno = tapeno * 10 + ( c - '0' );
        c = s[ i++ ];
    }

/* have seen ( i - 1 ) digits */
/* c is s[ i - 1 ] is not a digit */

    if ( i < l && c == '.' && ( s[ 0 ] != '0' || i <= 2 ) ) {
        l -= i;
        copymem( l, s + i, s );
        s[ l ] = '\0';
    } else {
        tapeno = -1;
    }

    Q-> Q_tapeno = tapeno;
    Q-> Q_length = l;
    Q-> Q_cstr = s;
    Sfree( (char *) P );
    return( Q );
}

void Q_destroy( Q_OBJECT Q )
{
    Sfree( (char *) Q-> Q_cstr );
    Sfree( (char *) Q );
}

int Q_tapeno( Q_OBJECT Q )
{
    return( Q-> Q_tapeno );
}

int Q_length( Q_OBJECT Q )
{
    return( Q-> Q_length );
}

char *Q_cstr( Q_OBJECT Q )
{
    return( Q-> Q_cstr );
}
