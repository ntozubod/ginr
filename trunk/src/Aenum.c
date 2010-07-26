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

static A_OBJECT		GAe;
static T_OBJECT		GTe;
static SHORT		*e_vec;
static int		*c_vec;
static int		e_lev;
static int		en_cnt;
static int		en_max;
static char *		en_str;

int A_en_DFS( state )
SHORT state;
{
	A_row		*p;
	static SHORT	i;

	for( p = GAe-> A_p[state]; p < GAe-> A_p[state+1]; ++p )
		if ( p-> A_b == 1 ) {
			fprintf( fpout, "    " );
			if ( e_lev == 0 ) fprintf( fpout, "^ " );
			else for( i = 0; i < e_lev; i++ ) {
				++en_cnt;
				if ( GAe-> A_nT == 1 ) {
					en_str = T_name( GTe, (int) e_vec[i] );
					if ( *en_str == '\n' )
						en_str = "\\n";
					else if ( *en_str == '\t' )
						en_str = "\\t";
					else if ( *en_str == ' ' )
						en_str = "\\_";
					else if ( *en_str == '\\' )
						en_str = "\\\\";
					fprintf( fpout, "%s ", en_str );
				} else {
					en_str = T_name( GTe, (int) e_vec[i]
							/ GAe-> A_nT );
					if ( *en_str == '\n' )
						en_str = "\\n";
					else if ( *en_str == '\t' )
						en_str = "\\t";
					else if ( *en_str == ' ' )
						en_str = "\\_";
					else if ( *en_str == '\\' )
						en_str = "\\\\";
					fprintf( fpout, "%1d.%s ",
						e_vec[i] % GAe-> A_nT,
						en_str );
				}
			}
			fprintf( fpout, "\n" );
		} else {
			if ( e_lev == en_max ) return( 1 );
			if ( en_cnt > en_max ) return( 1 );
			e_vec[ e_lev++ ] = p-> A_b;
			if ( A_en_DFS( p-> A_c ) ) return( 1 );
			--e_lev;
		}
	return( 0 );
}

A_OBJECT A_enum( A, T, max )
A_OBJECT A;
T_OBJECT T;
int max;
{
	A_OBJECT Ar, Am;

	if ( A == NULL ) Error( "A_enum: No OBJECT" );

	Ar = A_copy( A );

	en_max = max;
	e_vec = s_alloc( en_max );
	e_lev = 0;
	GTe = T;
	en_cnt = 0;

	while( Ar-> A_nrows > 0 ) {
		Am = A_subs( A_lenmin( A_copy( Ar ) ) );
		GAe = Am;
		if ( A_en_DFS( START ) ) {
			fprintf( fpout, "Enum terminated\n" );
			A_destroy( Am );
			break;
		}
		Ar = A_differ( Ar, Am );
	}
	Sfree( (char *) e_vec );
	A_destroy( Ar );
	return( A );
}

int A_cd_DFS( state )
SHORT state;
{
	A_row		*p;
	static int	i;
	int		count;

	count = c_vec[ state ];
	if ( count != 0 ) return( count );
	c_vec[ state ] = (-1);
	for( p = GAe-> A_p[state]; p < GAe-> A_p[state+1]; ++p )
		if ( p-> A_b == 1 ) ++count;
		else {
			i = A_cd_DFS( p-> A_c );
			if ( i < 0 ) return( -1 );
			count += i;
		}
	return( c_vec[ state ] = count );
}

int A_card( A )
A_OBJECT A;
{
	int i;
	if ( A == NULL ) Error( "A_card: No OBJECT" );

	A = A_subs( A );
	c_vec = i_alloc( A-> A_nQ );
	for( i = 0; i < A-> A_nQ; i++ ) c_vec[ i ] = 0;
	GAe = A;
	i = A_cd_DFS( START );
	Sfree( (char *) c_vec );
	return( i );
}

A_OBJECT A_pref( A )
A_OBJECT A;
{
	int i;
	A = A_open( A_min( A ) );
	for ( i = A-> A_nQ; --i >= 2; )
		A = A_add( A, i, 1, 1 );
	A = A_add( A, 0, 1, 1 );
	A = A_close( A );
	A-> A_mode = DFA;
	return( A );
}

A_OBJECT A_suff( A )
A_OBJECT A;
{
	int i;
	A = A_open( A_min( A ) );
	for ( i = A-> A_nQ; --i >= 2; )
		A = A_add( A, 0, 0, i );
	A = A_add( A, 0, 1, 1 );
	A = A_close( A );
	A-> A_mode = NFA_CLOSED;
	return( A );
}
