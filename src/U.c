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

static SHORT *U_hashpos = 0;
static int U_calls = 0;
static int U_probes = 0;
static int U_fail = 0;

U_OBJECT U_create( )
{
  register U_OBJECT U;
  U = ( U_OBJECT ) Salloc( sizeof( struct U_desc ) );
  U-> Type = U_Object;
  U-> U_n  = 0;
  U-> U_lrec = 0;
  U-> U_lhash = 1;
  U-> U_rec = 0;
  U-> U_hash = s_alloc( 1 );
  U-> U_hash[ 0 ] = MAXSHORT;
  return( U );
}

void U_destroy( U )
register U_OBJECT U;
{
  if ( U == NULL ) {
    return;
  }

  Sfree( ( char * ) U-> U_rec );
  Sfree( ( char * ) U-> U_hash );
  Sfree( ( char * ) U );
}

int U_member( U, reca, recb, recc )
register U_OBJECT U;
register int reca, recb, recc;
{
  register SHORT *p;
  ++U_calls;
  p  = U-> U_hash +
       ( ( ( ( 16807 * ( ( 16807 * reca + recb ) & 017777777777 ) + recc )
             & 017777777777 ) * 16807 ) & 017777777777 )
       % U-> U_lhash;

  while ( *p < MAXSHORT ) {
    ++U_probes;

    if ( U-> U_rec[*p].A_a == reca
         && U-> U_rec[*p].A_b == recb
         && U-> U_rec[*p].A_c == recc ) {
      return( *p );
    }

    if ( --p < U-> U_hash ) {
      p = U-> U_hash + U-> U_lhash - 1;
    }
  }

  ++U_fail;
  U_hashpos = p;
  return( -1 );
}

U_OBJECT U_grow( U, lrec )
register U_OBJECT U;
int lrec;
{
  register SHORT *p, *pl;
  register A_row *q, *ql;
  register int i;

  if ( lrec < 15 ) {
    lrec = 15;
  }

  if ( lrec <= U-> U_lrec ) {
    return( U );
  }

  Sfree( ( char * ) U-> U_hash );
  U-> U_rec =
    ( A_row * ) Srealloc( ( char * ) U-> U_rec,
                          lrec * sizeof( A_row ) );
  U-> U_lrec = Ssize( ( char * ) U-> U_rec ) / sizeof( A_row );

  if ( U-> U_lrec > MAXSHORT ) {
    U-> U_lrec = MAXSHORT;
  }

  U-> U_hash = s_alloc( 2 * U-> U_lrec );
  U-> U_lhash = Ssize( ( char * ) U-> U_hash ) / sizeof( SHORT );
  p = U-> U_hash;
  pl = p + U-> U_lhash;

  while ( p < pl ) {
    *p++ = MAXSHORT;
  }

  q = U-> U_rec;
  ql = q + U-> U_n;
  i = 0;

  while ( q < ql ) {
    if ( U_member( U, ( int )q-> A_a, ( int )q-> A_b, ( int )q-> A_c )
         != ( -1 ) ) {
      Error( "U_grow: BOTCH" );
    }

    ++q;
    *U_hashpos = i++;
  }

  return( U );
}

int U_insert( U, reca, recb, recc )
register U_OBJECT U;
register int reca, recb, recc;
{
  register int i;

  if ( U-> U_n >= U-> U_lrec ) {
    if ( U-> U_n >= MAXSHORT ) {
      Error( "U_insert: Table FULL" );
    }

    U = U_grow( U, 2 * U-> U_lrec );
  }

  if ( ( i = U_member( U, reca, recb, recc ) ) >= 0 ) {
    return( i );
  }

  U-> U_rec[ U-> U_n ].A_a = reca;
  U-> U_rec[ U-> U_n ].A_b = recb;
  U-> U_rec[ U-> U_n ].A_c = recc;
  return( *U_hashpos = U-> U_n++ );
}

A_row *U_rec( U, i )
register U_OBJECT U;
register int i;
{
  if ( i >= 0 && i < U-> U_n ) {
    return( U-> U_rec + i );

  } else        {
    return( NULL );
  }
}

void U_stats()
{
  fprintf( fpout, "(U) Calls:%7d  Probes:%7d  Unsuccessful:%7d\n",
           U_calls, U_probes, U_fail );
}
