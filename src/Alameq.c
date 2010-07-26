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

#include <stdio.h>

extern FILE *fpout;

#include "O.h"

#define UNMARK		MAXSHORT
#define DELETED		(MAXSHORT-1)

static A_OBJECT GAl;
static SHORT *l_stk, *l_low;
static int l_cnt, l_top, l_reopen;

int A_la_DFS( state )
int state;
{
	static int next;
	int dfn;
	A_row *p, *pz;
	l_low[ state ] = dfn = l_cnt++;
	l_stk[ state ] = l_top;
	l_top = state;
	pz = GAl-> A_p[ state + 1 ];
	for( p = GAl-> A_p[state]; p < pz && p-> A_b == 0; p++ ) {
		next = p-> A_c;
		if ( l_stk[ next ] == UNMARK ) {
			next = A_la_DFS( next );
			if ( l_low[next] < l_low[state] )
				l_low[state] = l_low[next];
		} else	if ( l_stk[next] < DELETED
			&&   l_low[next] <= l_low[state] ) {
				l_low[state] = l_low[next];
				l_reopen = 1;
		}
	}
	if ( l_low[ state ] == dfn )
		for( next = (-1); next != state; ) {
			l_top = l_stk[ next = l_top ];
			l_stk[ next ] = DELETED;
			l_low[ next ] = dfn;
		}
	return( state );
}

A_OBJECT A_lameq( A )
register A_OBJECT A;
{
	register int i;

	if ( A == NULL ) Error( "A_lameq: No OBJECT" );
	if ( A-> A_nQ >= DELETED ) Error( "A_lameq: Too many states" );
	if ( A-> A_mode <  NFA_TRIM ) A = A_trim( A );
	if ( A-> A_mode >= NFA_EQLAM ) return( A );
	if ( A-> A_nrows == 0 ) {
		A-> A_mode = NFA_EQLAM;
		return( A );
	}

	if ( A_report ) fprintf( fpout, "--> A_lameq\n" );
	l_stk = s_alloc( A-> A_nQ );
	l_low = s_alloc( A-> A_nQ );
	l_cnt = 0;
	l_top = 0;
	l_reopen = 0;
	for( i = A-> A_nQ; --i >= 0; ) l_stk[i] = UNMARK;
	GAl = A;
	for( i = A-> A_nQ; --i >= 0; )
		if ( l_stk[i] == UNMARK )
			if ( A_la_DFS( i ) != i )
				Error( "A_lameq: BOTCH" );
	Sfree( (char *) l_stk );
	if ( l_reopen ) A = A_rename( A, l_low );
	Sfree( (char *) l_low );
	A-> A_mode = NFA_EQLAM;
	if ( A_report ) {
		fprintf( fpout, "<-- A_lameq  " );
		(void) A_rept( A );
	}
	return( A );
}
