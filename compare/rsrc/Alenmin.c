/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Not for sale or distribution without written permission of the author.
 */

#include <stdio.h>

#include "O.h"

A_OBJECT A_lenmin( A )
register A_OBJECT A;
{
	register A_row *lo, *hi, *p;
	int i, l, cur, dead;
	SHORT *bfs, *dis;

	if ( A-> A_nrows == 0 ) return( A );

	A = A_subs( A );

	bfs = s_alloc( A-> A_nQ );
	dis = s_alloc( A-> A_nQ );

	for ( i = 0; i < A-> A_nQ; i++ ) dis[i] = MAXSHORT;
	bfs[0] = START;
	l = 1;
	dis[START] = 0;

	for ( i = 0; i < l; i++ ) {
		cur = bfs[i];
		lo = A-> A_p[cur];
		hi = A-> A_p[cur+1];
		for ( p = lo; p < hi; p++ ) {
			if ( dis[p-> A_c] > dis[cur] + 1 ) {
				dis[p-> A_c] = dis[cur] + 1;
				bfs[l++] = p-> A_c;
			}
		}
	}

	A = A_open( A );
	lo = A-> A_t;
	hi = lo + A-> A_nrows;
	dead = A-> A_nQ++;
	for ( p = lo; p < hi; p++ )
		if ( dis[ p-> A_a ] >= dis[ p-> A_c ] ) p-> A_c = dead;

	Sfree( (char *) bfs );
	Sfree( (char *) dis );

	return( A_trim( A ) );
}

int A_minlen( A )
register A_OBJECT A;
{
	register A_row *lo, *hi, *p;
	int i, l, cur;
	SHORT *bfs, *dis;

	if ( A-> A_nrows == 0 ) return( -1 );

	A = A_subs( A );

	bfs = s_alloc( A-> A_nQ );
	dis = s_alloc( A-> A_nQ );

	for ( i = 0; i < A-> A_nQ; i++ ) dis[i] = MAXSHORT;
	bfs[0] = START;
	l = 1;
	dis[START] = 0;

	for ( i = 0; i < l; i++ ) {
		cur = bfs[i];
		lo = A-> A_p[cur];
		hi = A-> A_p[cur+1];
		for ( p = lo; p < hi; p++ ) {
			if ( dis[p-> A_c] == MAXSHORT ) {
				dis[p-> A_c] = dis[cur] + 1;
				bfs[l++] = p-> A_c;
			}
		}
	}

	i = dis[FINAL] - 1;

	Sfree( (char *) bfs );
	Sfree( (char *) dis );

	return( i );
}
