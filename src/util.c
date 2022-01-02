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

#include <stdio.h>

extern FILE *fpout;

#ifdef VAX
#include <sys/time.h>
#include <sys/resource.h>
char *sbrk();
#endif

Warning( s )
char *s;
{
    fprintf( fpout, "*** %s ***\n", s );
}

Error( s )
char *s;
{
    fprintf( fpout, "*** %s ***\n", s );
    fflush( fpout );
    exit( 1 );
}

SHORT *s_alloc( n )
int n;
{
    return( (SHORT *) Salloc( n * sizeof( SHORT ) ) );
}

int *i_alloc( n )
int n;
{
    return( (int *) Salloc( n * sizeof( int ) ) );
}

pr_time_diff()
{
#ifdef VAX
    static struct rusage *old = NULL, *new, *tmp;
    if ( old == NULL ) {
        old = (struct rusage *) Salloc( sizeof( struct rusage ) );
        new = (struct rusage *) Salloc( sizeof( struct rusage ) );
        (void) getrusage( 0, old );
        return;
    }
    (void) getrusage( 0, new );
    fprintf( fpout, "User%6d System%6d Storage%6dK\n",
             new-> ru_utime.tv_sec - old-> ru_utime.tv_sec,
             new-> ru_stime.tv_sec - old-> ru_stime.tv_sec,
             ( (int) sbrk(0) + 1023 ) / 1024 );
    tmp = old;
    old = new;
    new = tmp;
#else
    return;
#endif
}
