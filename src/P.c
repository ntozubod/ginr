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

P_OBJECT P_create( int length, char *cstr )
{
    P_OBJECT P;
    P = (P_OBJECT) Salloc( sizeof(struct P_desc) );
    P-> Type = P_Object;
    P-> P_length = length;
    P-> P_cstr = Salloc( length + 1 );
    copymem( length, cstr, P-> P_cstr );
    P-> P_cstr[ length ] = '\0';
    return( P );
}

void P_destroy( P_OBJECT P )
{
    Sfree( (char *) P-> P_cstr );
    Sfree( (char *) P );
}

int P_length( P_OBJECT P )
{
    return( P-> P_length );
}

char *P_cstr( P_OBJECT P )
{
    return( P-> P_cstr );
}
