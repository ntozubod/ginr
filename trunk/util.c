/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Not for sale or distribution without written permission of the author.
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
