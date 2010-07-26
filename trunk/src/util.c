/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Not for sale or distribution without written permission of the author.
 */

#include "O.h"

#include <stdio.h>
#include <stdlib.h>

extern FILE *fpout;

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
	return;
}
