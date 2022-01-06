#include <stdio.h>
#include "O.h"
#define MAX_TOKEN       1024
#ifdef CMS
char new [ 100 ] ;
char * cnvrtn ( name ) char * name ;
{
  char * p ;
  strcpy ( new, name ) ;

  if ( index ( new, ' ' ) == 0 ) {
    strcat ( new, " file" ) ;
  }

  for ( p = new ;
        * p ;
        p ++ ) if ( * p == '_' ) {
      * p = ' ' ;
    }

  return ( new ) ;
}
#else
#define cnvrtn(name) name
#endif
static FILE * fp ;
FILE * fpin, * fpout ;
extern FILE * fopen ( ) ;
static int c ;
char * get_name ( )
{
  int i ;
  static char token [ MAX_TOKEN ] ;

  while ( c == ' ' || c == '\t' ) {
    c = getc ( fp ) ;
  }

  i = 0 ;

  while ( c != ' ' && c != '\t' && c != '\n' ) {
    if ( c == EOF ) {
      Warning ( "get_name: Unexpected EOF" ) ;
      return ( NULL ) ;
    }

    if ( c == '\\' ) {
      c = getc ( fp ) ;

      switch ( c ) {
      case 'n' :
        c = '\n' ;
        break ;

      case 't' :
        c = '\t' ;
        break ;

      case '_' :
        c = ' ' ;
        break ;
      }
    }

    token [ i ++ ] = c ;
    c = getc ( fp ) ;

    if ( i >= MAX_TOKEN ) {
      Warning ( "get_name: Name too long" ) ;
      return ( NULL ) ;
    }
  }

  if ( i == 0 ) {
    Warning ( "get_name: Zero length name" ) ;
    return ( NULL ) ;
  }

  token [ i ] = 0 ;
  return ( token ) ;
}
void put_name ( str ) char * str ;
{
  int i ;

  while ( i = * str ++ ) {
    switch ( i ) {
    case ' ' :
      putc ( '\\', fp ) ;
      i = '_' ;
      break ;

    case '\n' :
      putc ( '\\', fp ) ;
      i = 'n' ;
      break ;

    case '\t' :
      putc ( '\\', fp ) ;
      i = 't' ;
      break ;

    case '\\' :
      putc ( '\\', fp ) ;
      break ;
    }

    putc ( i, fp ) ;
  }
}
int get_nl ( )
{
  while ( c == ' ' || c == '\t' ) {
    c = getc ( fp ) ;
  }

  if ( c != '\n' ) {
    Warning ( "get_nl: New line expected" ) ;
    return ( 0 ) ;
  }

  c = getc ( fp ) ;
  return ( 1 ) ;
}
A_OBJECT A_load ( file, T_Sigma ) char * file ;
T_OBJECT T_Sigma ;
{
  int from, symb, to, tape, ntapes, i ;
  A_OBJECT A ;
  T_OBJECT TQ ;
  char * t ;
  A_row * p ;

  if ( file != NULL ) {
    fp = fopen ( cnvrtn ( file ), "r" ) ;

  } else if ( fpin != NULL ) {
    fp = fpin ;

  } else {
    fp = stdin ;
  }

  if ( fp == NULL ) {
    Warning ( "File does not exist" ) ;
    return ( NULL ) ;
  }

  A = A_create ( ) ;

  if ( T_Sigma == NULL || T_insert ( T_Sigma, "^^" ) != 0 || T_insert ( T_Sigma, "-|" ) != 1 ) {
    Error ( "A_load: BOTCH 1" ) ;
  }

  A -> A_nT = ntapes = 1 ;
  c = getc ( fp ) ;

  if ( c >= 10 ) {
    TQ = T_create ( ) ;

    if ( T_insert ( TQ, "(START)" ) != START || T_insert ( TQ, "(FINAL)" ) != FINAL ) {
      Error ( "A_load: BOTCH 2" ) ;
    }

    while ( c != EOF ) {
      if ( ( t = get_name ( ) ) == NULL ) {
        A_destroy ( A ) ;
        T_destroy ( TQ ) ;
        return ( NULL ) ;
      }

      from = T_insert ( TQ, t ) ;

      if ( ( t = get_name ( ) ) == NULL ) {
        A_destroy ( A ) ;
        T_destroy ( TQ ) ;
        return ( NULL ) ;
      }

      if ( t [ 1 ] == '.' && t [ 0 ] >= '0' && t [ 0 ] <= '9' ) {
        tape = t [ 0 ] - '0' ;

        if ( tape >= ntapes ) {
          for ( p = A -> A_t + A -> A_nrows ;
                -- p >= A -> A_t ;
              ) {
            i = p -> A_b ;

            if ( i > 1 ) {
              i = i / ntapes * ( tape + 1 ) + i % ntapes ;
            }

            p -> A_b = i ;
          }

          A -> A_nT = ntapes = tape + 1 ;
        }

        symb = T_insert ( T_Sigma, t + 2 ) ;

        if ( symb == 1 && ntapes > 1 ) {
          A -> A_ems = 1 ;
        }

        symb = symb * ntapes + tape ;

      } else {
        symb = T_insert ( T_Sigma, t ) ;

        if ( symb == 1 && ntapes > 1 ) {
          A -> A_ems = 1 ;
        }

        if ( symb != 1 ) {
          symb *= ntapes ;
        }
      }

      if ( ( t = get_name ( ) ) == NULL ) {
        A_destroy ( A ) ;
        T_destroy ( TQ ) ;
        return ( NULL ) ;
      }

      to = T_insert ( TQ, t ) ;
      A = A_add ( A, from, symb, to ) ;

      if ( ! get_nl ( ) ) {
        A_destroy ( A ) ;
        T_destroy ( TQ ) ;
        return ( NULL ) ;
      }
    }

    if ( file != NULL ) {
      fclose ( fp ) ;
    }

    T_destroy ( TQ ) ;
    return ( A_close ( A_rename ( A, ( SHORT * ) NULL ) ) ) ;

  } else {
    A -> A_nT = ntapes = c + 1 ;
    c = getc ( fp ) ;

    if ( ! get_nl ( ) ) {
      A_destroy ( A ) ;
      return ( NULL ) ;
    }

    while ( c != EOF ) {
#ifdef CMS

      if ( c == '\\' ) {
        c = getc ( fp ) ;
      }

#endif
      from = ( c & 0377 ) * 256 ;
      c = getc ( fp ) ;
#ifdef CMS

      if ( c == '\\' ) {
        c = getc ( fp ) ;
      }

#endif
      from += c & 0377 ;
      c = getc ( fp ) ;
#ifdef CMS

      if ( c == '\\' ) {
        c = getc ( fp ) ;
      }

#endif
      to = ( c & 0377 ) * 256 ;
      c = getc ( fp ) ;
#ifdef CMS

      if ( c == '\\' ) {
        c = getc ( fp ) ;
      }

#endif
      to += c & 0377 ;
      c = getc ( fp ) ;

      if ( ( t = get_name ( ) ) == NULL ) {
        A_destroy ( A ) ;
        return ( NULL ) ;
      }

      if ( t [ 1 ] == '.' && t [ 0 ] >= '0' && t [ 0 ] <= '9' ) {
        tape = t [ 0 ] - '0' ;

        if ( tape >= ntapes ) {
          for ( p = A -> A_t + A -> A_nrows ;
                -- p >= A -> A_t ;
              ) {
            i = p -> A_b ;

            if ( i > 1 ) {
              i = i / ntapes * ( tape + 1 ) + i % ntapes ;
            }

            p -> A_b = i ;
          }

          A -> A_nT = ntapes = tape + 1 ;
        }

        symb = T_insert ( T_Sigma, t + 2 ) ;

        if ( symb == 1 && ntapes > 1 ) {
          A -> A_ems = 1 ;
        }

        symb = symb * ntapes + tape ;

      } else {
        symb = T_insert ( T_Sigma, t ) ;

        if ( symb == 1 && ntapes > 1 ) {
          A -> A_ems = 1 ;
        }

        if ( symb != 1 ) {
          symb *= ntapes ;
        }
      }

      A = A_add ( A, from, symb, to ) ;

      if ( ! get_nl ( ) ) {
        A_destroy ( A ) ;
        return ( NULL ) ;
      }
    }

    if ( file != NULL ) {
      fclose ( fp ) ;
    }

    A = A_close ( A ) ;
    A -> A_mode = DFA_MIN ;
    return ( A ) ;
  }
}
A_OBJECT A_store ( A, file, T_Sigma ) A_OBJECT A ;
char * file ;
T_OBJECT T_Sigma ;
{
  int t ;
  A_row * p, * pz ;

  if ( A == NULL || T_Sigma == NULL ) {
    return ( A ) ;
  }

  if ( file != NULL ) {
    if ( strcmp ( file, "devnull" ) == 0 ) {
      return ( A ) ;

    } else {
      fp = fopen ( cnvrtn ( file ), "w" ) ;
    }

  } else {
    fp = fpout ;

    if ( fp == NULL ) {
      fp = stdin ;
    }
  }

  if ( fp == NULL ) {
    Warning ( "Cannot open file" ) ;
    return ( A ) ;
  }

  pz = A -> A_t + A -> A_nrows ;

  for ( p = A -> A_t ;
        p < pz ;
        p ++ ) {
    if ( ( t = p -> A_a ) == START ) {
      fprintf ( fp, "(START) " ) ;

    } else if ( t == FINAL ) {
      fprintf ( fp, "(FINAL) " ) ;

    } else {
      fprintf ( fp, "%d ", t ) ;
    }

    if ( ( t = p -> A_b ) <= 1 || A -> A_nT == 1 ) {
      put_name ( T_name ( T_Sigma, t ) ) ;

    } else {
      fprintf ( fp, "%1d.", t % A -> A_nT ) ;
      put_name ( T_name ( T_Sigma, t / A -> A_nT ) ) ;
    }

    if ( ( t = p -> A_c ) == START ) {
      fprintf ( fp, " (START)\n" ) ;

    } else if ( t == FINAL ) {
      fprintf ( fp, " (FINAL)\n" ) ;

    } else {
      fprintf ( fp, " %d\n", t ) ;
    }
  }

  if ( file != NULL ) {
    fclose ( fp ) ;

  } else if ( fflush ( stdout ) == EOF ) {
    Error ( "A_store: fflush" ) ;
  }

  return ( A ) ;
}
A_OBJECT A_save ( A, file, T_Sigma ) A_OBJECT A ;
char * file ;
T_OBJECT T_Sigma ;
{
  int t ;
  A_row * p, * pz ;

  if ( A == NULL || T_Sigma == NULL ) {
    return ( A ) ;
  }

  if ( file != NULL ) {
    if ( strcmp ( file, "devnull" ) == 0 ) {
      return ( A ) ;

    } else {
      fp = fopen ( cnvrtn ( file ), "w" ) ;
    }

  } else {
    fp = fpout ;

    if ( fp == NULL ) {
      fp = stdin ;
    }
  }

  if ( fp == NULL ) {
    Warning ( "Cannot open file" ) ;
    return ( A ) ;
  }

  A = A_min ( A ) ;
  ( void ) putc ( A -> A_nT - 1, fp ) ;
  ( void ) putc ( '\n', fp ) ;
  pz = A -> A_t + A -> A_nrows ;

  for ( p = A -> A_t ;
        p < pz ;
        p ++ ) {
    t = ( p -> A_a / 256 ) & 0377 ;
#ifdef CMS

    if ( t == '\\' || t == '\n' ) {
      ( void ) putc ( '\\', fp ) ;
    }

#endif
    ( void ) putc ( ( char ) ( t ), fp ) ;
    t = p -> A_a % 256 ;
#ifdef CMS

    if ( t == '\\' || t == '\n' ) {
      ( void ) putc ( '\\', fp ) ;
    }

#endif
    ( void ) putc ( ( char ) ( t ), fp ) ;
    t = ( p -> A_c / 256 ) & 0377 ;
#ifdef CMS

    if ( t == '\\' || t == '\n' ) {
      ( void ) putc ( '\\', fp ) ;
    }

#endif
    ( void ) putc ( ( char ) ( t ), fp ) ;
    t = p -> A_c % 256 ;
#ifdef CMS

    if ( t == '\\' || t == '\n' ) {
      ( void ) putc ( '\\', fp ) ;
    }

#endif
    ( void ) putc ( ( char ) ( t ), fp ) ;

    if ( ( t = p -> A_b ) <= 1 || A -> A_nT == 1 ) {
      put_name ( T_name ( T_Sigma, t ) ) ;

    } else {
      fprintf ( fp, "%1d.", t % A -> A_nT ) ;
      put_name ( T_name ( T_Sigma, t / A -> A_nT ) ) ;
    }

    ( void ) putc ( '\n', fp ) ;
  }

  if ( file != NULL ) {
    fclose ( fp ) ;

  } else if ( fflush ( stdout ) == EOF ) {
    Error ( "A_save: fflush" ) ;
  }

  return ( A ) ;
}
A_OBJECT A_lwds ( file, T_Sigma ) char * file ;
T_OBJECT T_Sigma ;
{
  A_OBJECT A, As ;
  char * p, t [ 2 ] ;
  int i, nQ ;

  if ( file != NULL ) {
    fp = fopen ( cnvrtn ( file ), "r" ) ;

  } else if ( fpin != NULL ) {
    fp = fpin ;

  } else {
    fp = stdin ;
  }

  if ( fp == NULL ) {
    Warning ( "File does not exist" ) ;
    return ( NULL ) ;
  }

  if ( T_Sigma == NULL || T_insert ( T_Sigma, "^^" ) != 0 || T_insert ( T_Sigma, "-|" ) != 1 ) {
    Error ( "A_lwds: BOTCH 1" ) ;
  }

  A = A_create ( ) ;
  As = A_create ( ) ;
  t [ 1 ] = 0 ;
  c = getc ( fp ) ;

  while ( c != EOF ) {
    if ( A -> A_nQ > 64000 || A -> A_nrows > 100000 ) {
      fprintf ( fpout, "Pause to compress ...\n" ) ;
      A = A_close ( A ) ;
      A -> A_mode = NFA_CLOSED ;
      As = A_union ( A_min ( A ), A_min ( As ) ) ;
      As = A_close ( As ) ;
      As -> A_mode = NFA_CLOSED ;
      A = A_create ( ) ;
      fprintf ( fpout, "Done Compress (states %d transitions %d)\n", As -> A_nQ, As -> A_nrows ) ;
    }

    nQ = A -> A_nQ ;

    if ( ( p = get_name ( ) ) == NULL ) {
      A_destroy ( A ) ;
      A_destroy ( As ) ;
      return ( NULL ) ;
    }

    for ( i = 0 ;
          p [ i ] != 0 ;
          ++ i ) {
      t [ 0 ] = p [ i ] ;
      A = A_add ( A, ( i == 0 ) ? 0 : nQ + i - 1, T_insert ( T_Sigma, t ), nQ + i ) ;
    }

    A = A_add ( A, nQ + i - 1, 1, 1 ) ;

    if ( ! get_nl ( ) ) {
      A_destroy ( A ) ;
      A_destroy ( As ) ;
      return ( NULL ) ;
    }
  }

  if ( file != NULL ) {
    fclose ( fp ) ;
  }

  A = A_close ( A ) ;
  A -> A_mode = NFA_CLOSED ;
  As = A_union ( A_min ( A ), A_min ( As ) ) ;
  As = A_close ( As ) ;
  As -> A_mode = NFA_CLOSED ;
  return ( As ) ;
}
A_OBJECT A_prsseq ( A, file, T_Sigma ) A_OBJECT A ;
char * file ;
T_OBJECT T_Sigma ;
{
  int t ;
  A_row * p, * pz ;
  int i, n_read, n_write ;
  int ss_states = 0 ;

  if ( A == NULL || T_Sigma == NULL ) {
    return ( A ) ;
  }

  if ( A -> A_nT != 2 ) {
    Error ( "A_prsseq: Not two tapes" ) ;
  }

  if ( A -> A_mode < SSEQ ) {
    A = A_min ( A_sseq ( A ) ) ;
  }

  if ( file != NULL ) {
    if ( strcmp ( file, "devnull" ) == 0 ) {
      return ( A ) ;

    } else {
      fp = fopen ( cnvrtn ( file ), "w" ) ;
    }

  } else {
    fp = fpout ;

    if ( fp == NULL ) {
      fp = stdin ;
    }
  }

  if ( fp == NULL ) {
    Warning ( "Cannot open file" ) ;
    return ( A ) ;
  }

  for ( i = 0 ;
        i < A -> A_nQ ;
        ++ i ) {
    n_read = 0 ;
    n_write = 0 ;
    pz = A -> A_p [ i + 1 ] ;

    for ( p = A -> A_p [ i ] ;
          p < pz ;
          p ++ ) {
      if ( p -> A_b % 2 == 0 ) {
        ++ n_read ;

      } else {
        ++ n_write ;
      }
    }

    if ( ( n_read > 0 && n_write > 0 ) || n_write > 1 ) {
      Error ( "A_prsseq: Not Subsequential" ) ;
    }

    if ( i == START || n_write == 0 ) {
      ++ ss_states ;
      pz = A -> A_p [ i + 1 ] ;

      for ( p = A -> A_p [ i ] ;
            p < pz ;
            p ++ ) {
        if ( ( t = p -> A_a ) == START ) {
          fprintf ( fp, "(START)  " ) ;

        } else if ( t == FINAL ) {
          fprintf ( fp, "(FINAL)  " ) ;

        } else {
          fprintf ( fp, "%-7d  ", t ) ;
        }

        if ( n_write == 0 ) {
          put_name ( T_name ( T_Sigma, p -> A_b / 2 ) ) ;
          t = p -> A_c ;
          fprintf ( fp, " [" ) ;

        } else {
          t = p -> A_a ;
          fprintf ( fp, "[" ) ;
        }

        while ( A -> A_p [ t ] + 1 == A -> A_p [ t + 1 ] && A -> A_p [ t ] -> A_b % 2 == 1 ) {
          if ( A -> A_p [ t ] -> A_b != 1 ) {
            fprintf ( fp, " " ) ;
            put_name ( T_name ( T_Sigma, A -> A_p [ t ] -> A_b / 2 ) ) ;
          }

          t = A -> A_p [ t ] -> A_c ;
        }

        fprintf ( fp, " ]  " ) ;
#ifndef CMS
        fprintf ( fp, "\t" ) ;
#endif

        if ( t == START ) {
          fprintf ( fp, "(START)\n" ) ;

        } else if ( t == FINAL ) {
          fprintf ( fp, "(FINAL)\n" ) ;

        } else {
          fprintf ( fp, "%d\n", t ) ;
        }
      }
    }
  }

  fprintf ( fp, "\n%d states in subsequential transducer\n", ss_states ) ;

  if ( fp != stdout ) {
    printf ( "%d states in subsequential transducer\n", ss_states ) ;
  }

  if ( file != NULL ) {
    fclose ( fp ) ;

  } else if ( fflush ( stdout ) == EOF ) {
    Error ( "A_store: fflush" ) ;
  }

  return ( A ) ;
}
