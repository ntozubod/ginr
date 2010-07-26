/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Not for sale or distribution without written permission of the author.
 */

#include <stdio.h>

extern FILE *fpout;

#include "O.h"

static SHORT *R_hashpos = 0;
static int R_calls = 0;
static int R_probes = 0;
static int R_fail = 0;

R_OBJECT R_create( )
{
	register R_OBJECT R;
	R = (R_OBJECT) Salloc( sizeof(struct R_desc) );
	R-> Type = R_Object;
	R-> R_n  = 0;
	R-> R_lrec = 0;
	R-> R_lhash = 1;
	R-> R_rec = 0;
	R-> R_hash = s_alloc( 1 );
	R-> R_hash[ 0 ] = MAXSHORT;
	return( R );
}

void R_destroy( R )
register R_OBJECT R;
{
	if ( R == NULL ) return;
	Sfree( (char *) R-> R_rec );
	Sfree( (char *) R-> R_hash );
	Sfree( (char *) R );
}

int R_member( R, reca, recb )
register R_OBJECT R;
register int reca, recb;
{
	register SHORT *p;
	++R_calls;
	p  = R-> R_hash
		+ ((((16807 * reca + recb) & 017777777777) * 16807 )
		& 017777777777 ) % R-> R_lhash;
	while ( *p < MAXSHORT ) {
		++R_probes;
		if ( R-> R_rec[*p].R_a == reca
		  && R-> R_rec[*p].R_b == recb ) return( *p );
		if ( --p < R-> R_hash )
			p = R-> R_hash + R-> R_lhash - 1;
	}
	++R_fail;
	R_hashpos = p;
	return( -1 );
}

R_OBJECT R_grow( R, lrec )
register R_OBJECT R;
int lrec;
{
	register SHORT *p, *pl;
	register R_row *q, *ql;
	register int i;
	if ( lrec < 15 ) lrec = 15;
	if ( lrec <= R-> R_lrec ) return( R );
	Sfree( (char *) R-> R_hash );
	R-> R_rec =
		(R_row *) Srealloc( (char *) R-> R_rec,
			lrec * sizeof(R_row) );
	R-> R_lrec = Ssize( (char *) R-> R_rec ) / sizeof(R_row);
	if ( R-> R_lrec > MAXSHORT ) R-> R_lrec = MAXSHORT;
	R-> R_hash = s_alloc( 2 * R-> R_lrec );
	R-> R_lhash = Ssize( (char *) R-> R_hash ) / sizeof(SHORT);
	p = R-> R_hash;
	pl = p + R-> R_lhash;
	while ( p < pl ) *p++ = MAXSHORT;
	q = R-> R_rec;
	ql = q + R-> R_n;
	i = 0;
	while ( q < ql ) {
		if ( R_member( R, (int)q-> R_a, (int)q-> R_b ) != (-1) )
			Error( "R_grow: BOTCH" );
		++q;
		*R_hashpos = i++;
	}
	return( R );
}

int R_insert( R, reca, recb )
register R_OBJECT R;
register int reca, recb;
{
	register int i;
	if ( R-> R_n >= R-> R_lrec ) {
		if ( R-> R_n >= MAXSHORT )
			Error( "R_insert: Table FULL" );
		R = R_grow( R, 2 * R-> R_lrec );
	}
	if ( (i = R_member( R, reca, recb )) >= 0 ) return( i );
	R-> R_rec[ R-> R_n ].R_a = reca;
	R-> R_rec[ R-> R_n ].R_b = recb;
	return( *R_hashpos = R-> R_n++ );
}

R_row *R_rec( R, i )
register R_OBJECT R;
register int i;
{
	if ( i >= 0 && i < R-> R_n ) return( R-> R_rec + i );
	else	return( NULL );
}

void R_stats()
{
	fprintf( fpout, "(R) Calls:%7d  Probes:%7d  Unsuccessful:%7d\n",
		R_calls, R_probes, R_fail );
}
