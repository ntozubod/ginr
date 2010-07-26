/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Not for sale or distribution without written permission of the author.
 */

#include <stdio.h>

extern FILE *fpout;

#include "O.h"

#define UNMARK		0
#define MARK		1
#define CONDEMN		2

static A_OBJECT GAc;
static A_OBJECT GAc2;
static SHORT *c_stk, *c_mark;
static int c_top;

A_cl_DFS( state )
int state;
{
	A_row *p, *pz;
	static int j, next;
	static A_row *lo, *hi, *mid;

	pz = GAc-> A_p[ state + 1 ];
	for( p = GAc-> A_p[ state ]; p < pz && p-> A_b == 0 ; p++ ) {
		next = p-> A_c;
		for( j = c_top; --j >= 0; ) {
			lo = GAc-> A_p[ c_stk[j] ];
			hi = GAc-> A_p[ c_stk[j] + 1 ] - 1;
			while ( lo <= hi ) {
				mid = lo + ( hi - lo ) / 2;
				if ( mid-> A_b != 0 || mid-> A_c > next )
					hi = mid - 1;
				else if ( mid-> A_c < next )
					lo = mid + 1;
				else	break;
			}
			if ( hi < lo ) GAc2 =
				A_add( GAc2, (int) c_stk[j], 0, next );
		}
		if ( c_mark[ state ] == UNMARK ) {
			c_stk[ c_top++ ] = state;
			A_cl_DFS( next );
			--c_top;
		} else	A_cl_DFS( next );
	}
	if ( c_mark[ state ] == UNMARK ) c_mark[ state ] = MARK;
}

A_OBJECT A_clsure( A )
register A_OBJECT A;
{
	register A_row *p, *pz;
	register int i, n_condemned;

	if ( A == NULL ) Error( "A_closure: No OBJECT" );
	if ( A-> A_mode <  NFA_CMLAM ) A = A_lamcm( A );
	if ( A-> A_mode >= NFA_CLOSED ) return( A );
	if ( A-> A_nrows == 0 ) {
		A-> A_mode = NFA_CLOSED;
		return( A );
	}

	if ( A_report ) fprintf( fpout, "--> A_closure\n" );
	c_stk = s_alloc( A-> A_nQ );
	c_mark = s_alloc( A-> A_nQ );
	c_top = 0;
	n_condemned = 0;
	c_mark[ START ] = UNMARK;
	c_mark[ FINAL ] = UNMARK;
	for( i = A-> A_nQ; --i >= 2; )
		if ( A-> A_p[i] != (p = A-> A_p[i+1])
		  && (p - 1)-> A_b == 0 ) {
			c_mark[i] = CONDEMN;
			++n_condemned;
		} else	c_mark[i] = UNMARK;
	if ( n_condemned > 0 ) {
		pz = A-> A_t + A-> A_nrows;
		for( p = A-> A_t; p < pz; p++ )
			if ( p-> A_b != 0 && c_mark[ p-> A_c ] == CONDEMN ) {
				c_mark[ p-> A_c ] = UNMARK;
				--n_condemned;
			}
	}
	GAc2 = A_create();
	GAc2-> A_nT = A-> A_nT;
	GAc = A;
	for( i = A-> A_nQ; --i >= START; )
		if ( c_mark[ i ] == UNMARK ) A_cl_DFS( i );
	Sfree( (char *) c_stk );
	if ( n_condemned > 0 ) {
		for( p = pz = A-> A_t + A-> A_nrows; --p > A-> A_t; )
			if ( c_mark[ p-> A_a ] == CONDEMN
			  || c_mark[ p-> A_c ] == CONDEMN ) {
				--pz;
				p-> A_a = pz-> A_a;
				p-> A_b = pz-> A_b;
				p-> A_c = pz-> A_c;
			}
		A-> A_nrows = pz - A-> A_t;
	}
	Sfree( (char *) c_mark );
	if ( n_condemned > 0 || GAc2-> A_nrows > 0 ) {
		A = A_open( A );
		pz = GAc2-> A_t + GAc2-> A_nrows;
		for( p = GAc2-> A_t; p < pz; p++ )
			A = A_add( A, (int)p-> A_a, 0, (int)p-> A_c );
		A = A_close( A );
	}
	A_destroy( GAc2 );
	A-> A_mode = NFA_CLOSED;
	if ( A_report ) {
		fprintf( fpout, "<-- A_closure  " );
		(void) A_rept( A );
	}
	return( A );
}
