/*
 * Copyright (c) 1985, J Howard Johnson, University of Waterloo.
 * Not for sale or distribution without written permission of the author.
 */

#include <stdio.h>

extern FILE *fpout;

#include "O.h"

void A_conform( A1, A2 )
register A_OBJECT A1, A2;
{
	register int i;
	register A_row *p;

	if ( A1-> A_nT < A2-> A_nT ) {
		for( p = A1-> A_t + A1-> A_nrows; --p >= A1-> A_t; ) {
			i = p-> A_b;
			if ( i > 1 ) i = i / A1-> A_nT * A2-> A_nT
					+ i % A1-> A_nT;
			p-> A_b = i;
		}
		A1-> A_nT = A2-> A_nT;
	} else if ( A1-> A_nT > A2-> A_nT ) {
		for( p = A2-> A_t + A2-> A_nrows; --p >= A2-> A_t; ) {
			i = p-> A_b;
			if ( i > 1 ) i = i / A2-> A_nT * A1-> A_nT
					+ i % A2-> A_nT;
			p-> A_b = i;
		}
		A2-> A_nT = A1-> A_nT;
	}
}

A_OBJECT A_phi()
{
	return( A_create() );
}

A_OBJECT A_lambda()
{
	return( A_add( A_create(), START, 1, FINAL ) );
}

A_OBJECT A_deecho( A, ECHO, NOECHO )
register A_OBJECT A;
int ECHO, NOECHO;
{
	register A_OBJECT A1;
	register A_row *p;
	int e, base;
	A = A_min( A );
	A1 = A_create();
	A1-> A_nT = A-> A_nT;
	e = A-> A_nQ;
	base = 2 * e - 1;

for( p = A-> A_t + A-> A_nrows; --p >= A-> A_t; ) {
    if ( p-> A_b == ECHO ) {
	A1 = A_add( A1,   (int)p-> A_a,            0, e+(int)p-> A_c );
	A1 = A_add( A1, e+(int)p-> A_a,            0, e+(int)p-> A_c );
    } else if ( p-> A_b == NOECHO ) {
	A1 = A_add( A1,   (int)p-> A_a,            0,   (int)p-> A_c );
	A1 = A_add( A1, e+(int)p-> A_a,            0,   (int)p-> A_c );
    } else if ( p-> A_b == 1 ) {
	A1 = A_add( A1,   (int)p-> A_a,            1,   (int)p-> A_c );
	A1 = A_add( A1, e+(int)p-> A_a,            1,   (int)p-> A_c );
    } else if ( p-> A_b % A-> A_nT == 0 ) {
	A1 = A_add( A1,   (int)p-> A_a, (int)p-> A_b,   (int)p-> A_c );
	A1 = A_add( A1, e+(int)p-> A_a, (int)p-> A_b,         ++base );
	A1 = A_add( A1, base,         (int)p-> A_b+1, e+(int)p-> A_c );
    } else {
	A1 = A_add( A1,   (int)p-> A_a, (int)p-> A_b,   (int)p-> A_c );
	A1 = A_add( A1, e+(int)p-> A_a, (int)p-> A_b, e+(int)p-> A_c );
    }
}

A_destroy( A );
return A1;
}

A_OBJECT A_letter( t, x )
int t, x;
{
	register A_OBJECT A;
	A = A_create();
	A-> A_nT = t + 1;
	if ( x == 1 && t == 0 ) A-> A_nT = 2;
	return( A_add( A_add( A, START, x * A-> A_nT + t, 2 ), 2, 1, FINAL ) );
}

A_OBJECT A_opt( A )
register A_OBJECT A;
{
	register int new_state;
	register A_row *p;

	A = A_open( A );
	new_state = A-> A_nQ;
	for( p = A-> A_t + A-> A_nrows; --p >= A-> A_t; ) {
		if ( p-> A_a == START ) p-> A_a = new_state;
		if ( p-> A_c == START ) p-> A_c = new_state;
	}
	A = A_add( A, START, 0, new_state );
	A = A_add( A, START, 1, FINAL );
	return( A );
}

A_OBJECT A_plus( A )
register A_OBJECT A;
{
	register int new_state;
	register A_row *p;

	A = A_open( A_trim( A ) );
	new_state = A-> A_nQ;
	for( p = A-> A_t + A-> A_nrows; --p >= A-> A_t; )
		if ( p-> A_b == 1 ) {
			p-> A_b = 0;
			p-> A_c = new_state;
		}
	A = A_add( A, new_state, 0, START );
	A = A_add( A, new_state, 1, FINAL );
	return( A );
}

A_OBJECT A_star( A )
A_OBJECT A;
{
	return( A_opt( A_plus( A ) ) );
}

A_OBJECT A_union( A1, A2 )
register A_OBJECT A1, A2;
{
	register int base, a, c;
	register A_row *p;

	A_conform( A1, A2 );
	A1 = A_open( A1 );
	base = A1-> A_nQ;
	for( p = A1-> A_t + A1-> A_nrows; --p >= A1-> A_t; ) {
		if ( p-> A_a == START ) p-> A_a = base + 1;
		if ( p-> A_c == START ) p-> A_c = base + 1;
	}
	for( p = A2-> A_t + A2-> A_nrows; --p >= A2-> A_t; ) {
		if ( (a = p-> A_a) != FINAL ) a += base;
		if ( (c = p-> A_c) != FINAL ) c += base;
		A1 = A_add( A1, a, (int) p-> A_b, c );
	}
	A1 = A_add( A_add( A1, START, 0, base ), START, 0, base + 1 );
	A_destroy( A2 );
	return( A1 );
}

A_OBJECT A_concat( A1, A2 )
register A_OBJECT A1, A2;
{
	register int base, a, c;
	register A_row *p;

	A_conform( A1, A2 );
	A1 = A_open( A1 );
	base = A1-> A_nQ - 1;
	for( p = A1-> A_t + A1-> A_nrows; --p >= A1-> A_t; )
		if ( p-> A_b == 1 && p-> A_c == FINAL ) {
			p-> A_b = 0;
			p-> A_c = base + 1;
		}
	A1-> A_nQ = base + 2;
	for( p = A2-> A_t + A2-> A_nrows; --p >= A2-> A_t; ) {
		if ( (a = p-> A_a) > FINAL ) a += base;
		else if ( a == START ) a = base + 1;
		if ( (c = p-> A_c) > FINAL ) c += base;
		else if ( c == START ) c = base + 1;
		A1 = A_add( A1, a, (int) p-> A_b, c );
	}
	A_destroy( A2 );
	return( A1 );
}

A_OBJECT A_intersect( A1, A2 )
register A_OBJECT A1, A2;
{
	register A_OBJECT A;
	int current;
	A_row *p1, *p1z, *p2, *p2z;
	R_OBJECT R;
	R_row *cur_st;

	A_conform( A1, A2 );
	A1 = A_min( A1 );
	A2 = A_min( A2 );
	A = A_create();
	A-> A_nT = A1-> A_nT;
	if ( A-> A_nT > 1 )
		fprintf( fpout,
			"Warning: & applied to multi-tape automaton\n" );
	R = R_create();

	if ( R_insert( R, START, START ) != START )
		Error( "A_intersect: BOTCH 1" );
	if ( R_insert( R, FINAL, FINAL ) != FINAL )
		Error( "A_intersect: BOTCH 2" );

	for( current = 0; current < R-> R_n; current++ ) {
		cur_st = R_rec( R, current );
		p1  = A1-> A_p[ cur_st-> R_a ];
		p1z = A1-> A_p[ cur_st-> R_a + 1 ];
		p2  = A2-> A_p[ cur_st-> R_b ];
		p2z = A2-> A_p[ cur_st-> R_b + 1 ];
		while( p1 < p1z && p2 < p2z ) {
			if ( p1-> A_b < p2-> A_b ) ++p1;
			else if ( p1-> A_b > p2-> A_b ) ++p2;
			else {
				A = A_add( A, current, (int)p1-> A_b,
					R_insert( R, (int)p1-> A_c,
						(int)p2-> A_c ) );
				++p1;
				++p2;
			}
		}
	}
	A_destroy( A1 );
	A_destroy( A2 );
	R_destroy( R );
	A = A_trim ( A );
	A-> A_mode = DFA;
	return( A );
}

A_OBJECT A_differ( A1, A2 )
register A_OBJECT A1, A2;
{
	register A_OBJECT A;
	int current, dead;
	A_row *p1, *p1z, *p2, *p2z;
	R_OBJECT R;
	R_row *cur_st;

	A_conform( A1, A2 );
	A1 = A_min( A1 );
	A2 = A_min( A2 );
	A = A_create();
	A-> A_nT = A1-> A_nT;
	if ( A-> A_nT > 1 )
		fprintf( fpout,
			"Warning: - applied to multi-tape automaton\n" );
	R = R_create();

	dead = A2-> A_nQ;
	if ( R_insert( R, START, START ) != START )
		Error( "A_differ: BOTCH 1" );
	if ( R_insert( R, FINAL, dead ) != FINAL )
		Error( "A_differ: BOTCH 2" );

	for( current = 0; current < R-> R_n; current++ ) {
		cur_st = R_rec( R, current );
		p1  = A1-> A_p[ cur_st-> R_a ];
		p1z = A1-> A_p[ cur_st-> R_a + 1 ];
		if ( cur_st-> R_b != dead ) {
			p2  = A2-> A_p[ cur_st-> R_b ];
			p2z = A2-> A_p[ cur_st-> R_b + 1 ];
			while( p1 < p1z ) {
				if ( p2 == p2z || p1-> A_b < p2-> A_b ) {
					A = A_add( A, current, (int)p1-> A_b,
						R_insert( R, (int)p1-> A_c,
							dead ) );
					++p1;
				}
				else if ( p2-> A_b < p1-> A_b ) ++p2;
				else {
					A = A_add( A, current, (int)p1-> A_b,
						R_insert( R, (int)p1-> A_c,
							(int) p2-> A_c ) );
					++p1;
					++p2;
				}
			}
		} else while( p1 < p1z ) {
			A = A_add( A, current, (int)p1-> A_b,
				R_insert( R, (int)p1-> A_c, dead ) );
			++p1;
		}
	}
	A_destroy( A1 );
	A_destroy( A2 );
	R_destroy( R );
	A = A_trim( A );
	A-> A_mode = DFA;
	return( A );
}

A_OBJECT A_xor( A1, A2 )
register A_OBJECT A1, A2;
{
	register A_OBJECT A;
	int current, dead;
	A_row *p1, *p1z, *p2, *p2z;
	R_OBJECT R;
	R_row *cur_st;

	A_conform( A1, A2 );
	A1 = A_min( A1 );
	A2 = A_min( A2 );
	A = A_create();
	A-> A_nT = A1-> A_nT;
	R = R_create();

	dead = A1-> A_nQ;
	if ( dead < A2-> A_nQ ) dead = A2-> A_nQ;
	if ( R_insert( R, START, START ) != START )
		Error( "A_xor: BOTCH 1" );
	if ( R_insert( R, dead, dead ) != FINAL )
		Error( "A_xor: BOTCH 2" );

	for( current = 0; current < R-> R_n; current++ ) {
		cur_st = R_rec( R, current );
		if ( cur_st-> R_a != dead ) {
			p1  = A1-> A_p[ cur_st-> R_a ];
			p1z = A1-> A_p[ cur_st-> R_a + 1 ];
			if ( cur_st-> R_b != dead ) {
				p2  = A2-> A_p[ cur_st-> R_b ];
				p2z = A2-> A_p[ cur_st-> R_b + 1 ];
				while( p1 < p1z || p2 < p2z ) {
					if ( p2 == p2z
					|| p1 < p1z && p1-> A_b < p2-> A_b ) {
						A = A_add( A, current,
							(int)p1-> A_b,
							R_insert( R,
							(p1-> A_c == FINAL) ?
							dead : (int)p1-> A_c,
							dead ) );
						++p1;
					} else if ( p1 == p1z
					|| p2 < p2z && p2-> A_b < p1-> A_b ) {
						A = A_add( A, current,
							(int)p2-> A_b,
							R_insert( R, dead,
							(p2-> A_c == FINAL) ?
							dead : (int)p2-> A_c
							) );
						++p2;
					} else {
						A = A_add( A, current,
							(int)p1-> A_b,
							R_insert( R,
							(int)p1-> A_c,
							(int)p2-> A_c ) );
						++p1;
						++p2;
					}
				}
			} else while( p1 < p1z ) {
				A = A_add( A, current, (int)p1-> A_b,
					R_insert( R,
						(p1-> A_c == FINAL) ?
						dead : (int)p1-> A_c, dead ) );
				++p1;
			}
		} else if ( cur_st-> R_b != dead ) {
			p2  = A2-> A_p[ cur_st-> R_b ];
			p2z = A2-> A_p[ cur_st-> R_b + 1 ];
			while( p2 < p2z ) {
				A = A_add( A, current, (int)p2-> A_b,
					R_insert( R, dead,
						(p2-> A_c == FINAL) ?
						dead : (int)p2-> A_c ) );
				++p2;
			}
		}
			
	}
	A_destroy( A1 );
	A_destroy( A2 );
	R_destroy( R );
	A = A_trim( A );
	A-> A_mode = DFA;
	return( A );
}

A_OBJECT A_alph( A )
register A_OBJECT A;
{
	register A_row *p;

	A = A_open( A_trim( A ) );
	for( p = A-> A_t + A-> A_nrows; --p >= A-> A_t; )
		if ( p-> A_b == 1 ) {
			p-> A_a = 2;
			p-> A_c = FINAL;
		} else if ( p-> A_b == 0 ) {
			p-> A_a = START;
			p-> A_c = 3;
		} else {
			p-> A_a = START;
			p-> A_c = 2;
			p-> A_b /= A-> A_nT;
		}
	A-> A_nT = 1;
	A-> A_nQ = 4;
	return( A_trim( A ) );
}

A_OBJECT A_rev( A )
register A_OBJECT A;
{
	register A_row *p;
	register int tmp;
	int new_state;
	int old_mode;

	old_mode = A-> A_mode;
	A = A_open( A );
	new_state = A-> A_nQ;
	for( p = A-> A_t + A-> A_nrows; --p >= A-> A_t; ) {
		tmp = p-> A_a;
		p-> A_a = p-> A_c;
		p-> A_c = tmp;
		if ( p-> A_b == 1 ) p-> A_b = 0;
		if ( p-> A_a == START ) p-> A_a = new_state;
		if ( p-> A_a == FINAL ) p-> A_a = START;
		if ( p-> A_c == START ) p-> A_c = new_state;
		if ( p-> A_c == FINAL ) p-> A_c = START;
	}
	A = A_add( A, new_state, 1, FINAL );
	if ( old_mode >= NFA_CLOSED ) {
		A = A_close( A );
		A-> A_mode = NFA_CLOSED;
	}
	return( A );
}

A_OBJECT A_shuffle( A1, A2 )
register A_OBJECT A1, A2;
{
	register A_OBJECT A;
	int current, cur_a, cur_b;
	A_row *p1, *p1z, *p2, *p2z;
	R_OBJECT R;
	R_row *cur_st;

	A_conform( A1, A2 );
	A1 = A_min( A1 );
	A2 = A_min( A2 );
	A = A_create();
	A-> A_nT = A1-> A_nT;
	R = R_create();

	if ( R_insert( R, START, START ) != START )
		Error( "A_shuffle: BOTCH 1" );
	if ( R_insert( R, FINAL, FINAL ) != FINAL )
		Error( "A_shuffle: BOTCH 2" );

	for( current = 0; current < R-> R_n; current++ ) {
		cur_st = R_rec( R, current );
		cur_a = cur_st-> R_a;
		cur_b = cur_st-> R_b;
		p1  = A1-> A_p[ cur_a ];
		p1z = A1-> A_p[ cur_a + 1 ];
		p2  = A2-> A_p[ cur_b ];
		p2z = A2-> A_p[ cur_b + 1 ];
		if ( p1 < p1z && p2 < p2z
		  && p1-> A_b == 1 && p2-> A_b == 1 )
			A = A_add( A, current, 1, FINAL );
		if ( p1 < p1z && p1-> A_b == 1 ) ++p1;
		if ( p2 < p2z && p2-> A_b == 1 ) ++p2;
		while( p1 < p1z ) {
			A = A_add( A, current, (int)p1-> A_b,
				R_insert( R, (int)p1-> A_c, cur_b ) );
			++p1;
		}
		while( p2 < p2z ) {
			A = A_add( A, current, (int)p2-> A_b,
				R_insert( R, cur_a, (int)p2-> A_c ) );
			++p2;
		}
	}
	A_destroy( A1 );
	A_destroy( A2 );
	R_destroy( R );
	A = A_close( A );
	A-> A_mode = NFA_CLOSED;
	return( A );
}
