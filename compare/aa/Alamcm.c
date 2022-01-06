#include <stdio.h>
extern FILE * fpout ;
#include "O.h"
#define NONE	MAXSHORT
#define FAIL	(MAXSHORT-1)
static SHORT * c_rena ;
int A_cm_DFS ( state ) int state ;
{
  if ( c_rena [ state ] == state ) {
    return ( state ) ;
  }

  if ( c_rena [ state ] >= FAIL ) {
    return ( state ) ;
  }

  return ( c_rena [ state ] = A_cm_DFS ( ( int ) c_rena [ state ] ) ) ;
}
A_OBJECT A_lamcm ( A ) A_OBJECT A ;
{
  A_row * p, * pz ;
  int i, j, n_combine ;

  if ( A == NULL ) {
    Error ( "A_lamcm: No OBJECT" ) ;
  }

  if ( A -> A_nQ >= FAIL ) {
    Error ( "A_lamcm: Too many states" ) ;
  }

  if ( A -> A_mode < NFA_EQLAM ) {
    A = A_lameq ( A ) ;
  }

  if ( A -> A_mode >= NFA_CMLAM ) {
    return ( A ) ;
  }

  if ( A_report ) {
    fprintf ( fpout, "--> A_lamcm\n" ) ;
  }

  c_rena = s_alloc ( A -> A_nQ ) ;
  n_combine = 1 ;

  while ( n_combine > 0 ) {
    for ( i = A -> A_nQ ;
          -- i >= 0 ;
        ) {
      c_rena [ i ] = NONE ;
    }

    n_combine = 0 ;

    for ( i = A -> A_nQ ;
          -- i >= 0 ;
        ) if ( A -> A_p [ i + 1 ] == ( p = A -> A_p [ i ] ) + 1 && p -> A_b == 0 ) {
        c_rena [ i ] = p -> A_c ;
        ++ n_combine ;
      }

    if ( n_combine == 0 ) {
      c_rena [ START ] = FAIL ;
      pz = A -> A_t + A -> A_nrows ;

      for ( p = A -> A_t ;
            p < pz ;
            p ++ ) if ( c_rena [ j = p -> A_c ] < FAIL ) {
          c_rena [ j ] = FAIL ;
          -- n_combine ;

        } else if ( p -> A_b != 0 ) {
          c_rena [ j ] = FAIL ;

        } else if ( c_rena [ j ] == NONE ) {
          c_rena [ j ] = p -> A_a ;
          ++ n_combine ;
        }
    }

    for ( i = A -> A_nQ ;
          -- i >= 0 ;
        ) if ( c_rena [ i ] < FAIL ) {
        c_rena [ i ] = A_cm_DFS ( ( int ) c_rena [ i ] ) ;
      }

    for ( i = A -> A_nQ ;
          -- i >= 0 ;
        ) if ( c_rena [ i ] >= FAIL ) {
        c_rena [ i ] = i ;
      }

    if ( n_combine > 0 ) {
      A = A_rename ( A, c_rena ) ;
    }
  }

  Sfree ( ( char * ) c_rena ) ;
  A -> A_mode = NFA_CMLAM ;

  if ( A_report ) {
    fprintf ( fpout, "<-- A_lamcm  " ) ;
    ( void ) A_rept ( A ) ;
  }

  return ( A ) ;
}
