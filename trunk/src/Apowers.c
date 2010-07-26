/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Not for sale or distribution without written permission of the author.
 */

#include "O.h"

A_OBJECT A_catpow( A, n )
register A_OBJECT A;
register int n;
{
	A_OBJECT Aprod;
	if ( n < 0 ) Error( "A_catpow: negative power" );
	A = A_min( A );
	if ( n % 2 )	Aprod = A_copy( A );
	else		Aprod = A_lambda();
	n /= 2;
	while( n > 0 ) {
		A = A_min( A_concat( A, A_copy( A ) ) );
		if ( n % 2 ) Aprod = A_min( A_concat( Aprod, A_copy( A ) ) );
		n /= 2;
	}
	A_destroy( A );
	return( Aprod );
}

A_OBJECT A_ident( A )
register A_OBJECT A;
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

A_OBJECT A_cmpow( A, n )
register A_OBJECT A;
register int n;
{
	A_OBJECT Aprod;
	if ( n < 0 ) Error( "A_cmpow: negative power" );
	A = A_min( A );
	if ( n % 2 )	Aprod = A_copy( A );
	else		Aprod = A_ident( A_copy( A ) );
	n /= 2;
	while( n > 0 ) {
		A = A_min( A_compose( A, A_copy( A ) ) );
		if ( n % 2 ) Aprod = A_min( A_compose( Aprod, A_copy( A ) ) );
		n /= 2;
	}
	A_destroy( A );
	return( Aprod );
}
