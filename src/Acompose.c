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

#include "O.h"

void A_compose_clsure_ctor();
void A_compose_clsure_dtor();
int A_compose_clsure( A_OBJECT, A_OBJECT, U_OBJECT, int, int, int );

A_OBJECT A_compose( A_OBJECT A1, A_OBJECT A2 )
{
    A_OBJECT A;
    int current, s1, s2, t1, t2, cur_a, cur_b, flag, flag2;
    t1 = 0; t2 = 0; /* Initialiaze to suppress warning JHJ */
    A_row *p1, *p1z, *p2, *p2z;
    U_OBJECT U;
    A_row *cur_st;

    if ( ! ( A1-> A_ems && A2-> A_ems ) ) {
        if ( A1-> A_ems ) A1 = A_deems( A1 );
        if ( A2-> A_ems ) A2 = A_deems( A2 );
    }
    if ( A1-> A_nT == 1 && A2-> A_nT == 1 ) {
        A_destroy( A1 );
        A_destroy( A2 );
        return( A_lambda() );
    }
    A1 = A_min( A1 );
    A2 = A_min( A2 );
    A = A_create();
    A-> A_nT = A1-> A_nT + A2-> A_nT - 2;
    U = U_create();

    if ( U_insert( U, START, START, 1 ) != START )
        Error( "A_compose: BOTCH 1" );
    if ( U_insert( U, FINAL, FINAL, 1 ) != FINAL )
        Error( "A_compose: BOTCH 2" );

    A_compose_clsure_ctor();

    for( current = 0; current < U-> U_n; current++ ) {
        cur_st = U_rec( U, current );
        cur_a = cur_st-> A_a;
        cur_b = cur_st-> A_b;
        flag  = cur_st-> A_c;
        flag2 = 0;
        if ( flag || A2-> A_nT > 1 ) {
            p2  = A2-> A_p[ cur_b ];
            p2z = A2-> A_p[ cur_b + 1 ];
            t2 = (-1);
            while ( p2 < p2z ) {
                if ( A2-> A_nT == 2 ) {
                    t2 = (p2-> A_b) & 1;
                } else {
                    t2 = (p2-> A_b) % (A2-> A_nT);
                }
                if ( p2-> A_b == 1 ) t2 = 0;
                if ( t2 == 0 ) break;
                ++p2;
            }
            if ( t2 != 0 ) flag2 = 1;
        }
        p1  = A1-> A_p[ cur_a ];
        p1z = A1-> A_p[ cur_a + 1 ];
        p2  = A2-> A_p[ cur_b ];
        p2z = A2-> A_p[ cur_b + 1 ];
        s1 = s2 = (-1);
        while( p1 < p1z || p2 < p2z ) {
            if ( s1 < 0 ) {
                if ( p1 < p1z ) {
                    if ( A1-> A_nT == 1 ) {
                        s1 = p1-> A_b;
                        t1 = 0;
                    } else if ( A1-> A_nT == 2 ) {
                        s1 = p1-> A_b;
                        t1 = s1 & 1;
                        s1 >>= 1;
                    } else {
                        s1 = p1-> A_b / A1-> A_nT;
                        t1 = p1-> A_b - s1 * A1-> A_nT;
                    }
                } else  s1 = MAXSHORT;
            }
            if ( s2 < 0 ) {
                if ( p2 < p2z ) {
                    if ( A2-> A_nT == 1 ) {
                        s2 = p2-> A_b;
                        t2 = 0;
                    } else if ( A2-> A_nT == 2 ) {
                        s2 = p2-> A_b;
                        t2 = s2 & 1;
                        s2 >>= 1;
                    } else {
                        s2 = p2-> A_b / A2-> A_nT;
                        t2 = p2-> A_b - s2 * A2-> A_nT;
                    }
                } else  s2 = MAXSHORT;
            }
            if ( p1-> A_b == 1 || p2-> A_b == 1 ) {
                if ( p1-> A_b == 1 && p2-> A_b == 1 )
                    A = A_add( A, current, 1, FINAL );
                if ( p1-> A_b == 1 ) {
                    ++p1;
                    s1 = (-1);
                }
                if ( p2-> A_b == 1 ) {
                    ++p2;
                    s2 = (-1);
                }
            } else if ( s1 <= s2 && t1 < A1-> A_nT - 1 ) {
                if ( !flag2 && s1 > 0 )
                    A = A_add( A, current,
                               ( A-> A_nT == 1 ? s1 :
                                 ( A-> A_nT == 2 ? (s1 << 1) + t1 :
                                   s1 * A-> A_nT + t1 ) ),
                               A_compose_clsure( A1, A2,
                                   U, (int)p1-> A_c, cur_b, 0 ) );
                ++p1;
                s1 = (-1);
            } else if ( s2 < s1 && t2 != 0 ) {
                if ( flag && s2 > 0 )
                    A = A_add( A, current,
                               ( A-> A_nT == 1 ? s2 :
                                 ( A-> A_nT == 2 ? (s2 << 1) :
                                   s2 * A-> A_nT ) ) + t2 + A1-> A_nT - 2,
                               A_compose_clsure( A1, A2,
                                   U, cur_a, (int)p2-> A_c, 1 ) );
                ++p2;
                s2 = (-1);
            } else if ( s1 == s2 && t1 == A1-> A_nT-1 && t2 == 0 ) {
                A = A_add( A, current, 0,
                    A_compose_clsure( A1, A2,
                        U, (int)p1-> A_c, (int)p2-> A_c, 1 ) );
                ++p1;
                ++p2;
                s1 = s2 = (-1);
            } else if ( s1 <= s2 ) {
                ++p1;
                s1 = (-1);
            } else {
                ++p2;
                s2 = (-1);
            }
        }
    }
    A_destroy( A1 );
    A_destroy( A2 );
    U_destroy( U );
    A_compose_clsure_dtor();
    return( A );
}

static int *map;
static int l_idx;
static int n_idx;
static int low_water;
static R_OBJECT R;

void A_compose_clsure_ctor() {
    l_idx = 100;
    n_idx = 0;
    low_water = 0;
    map = (int *) Salloc( l_idx * sizeof(int) );
    l_idx = Ssize( (char *) map ) / sizeof(int);
    R = R_create();
    return;
}

void A_compose_clsure_dtor() {
    l_idx = (-1);
    n_idx = 0;
    low_water = 0;
    Sfree( (char *) map );
    map = 0;
    R_destroy( R );
    return;
}

void A_compose_clsure_updt( int map_value ) {
    int i = 0;
    for ( i = low_water; i < n_idx; ++i ) {
        if ( map[ i ] == -1 ) {
            map[ i ] = map_value;
        }
    }
    low_water = n_idx;
}

int A_compose_clsure( A_OBJECT A1, A_OBJECT A2,
        U_OBJECT U, int state1, int state2, int flag ) {
    int s1, s2, t1, t2;
    A_row *p1, *p1z, *p2, *p2z;
    int candidate_state1 = 0;
    int candidate_state2 = 0;
    int candidate_flag   = 0;
    int found_one = 0;
    int idx = 0;
    int map_value = 0;
    int do_memoize = 0;

    for( ;; ) {

        do_memoize = ( state1 + state2 ) % 2 == 1;
        if ( do_memoize ) {
            idx = R_insert( R, state1, state2 );
            if ( l_idx <= idx ) {
                map = (int *) Srealloc( (char *) map, 2 * idx * sizeof(int) );
                l_idx = Ssize( (char *) map ) / sizeof(int);
            }
            assert( idx <= n_idx );
            while ( n_idx <= idx ) {
                map[ n_idx++ ] = (-1);
            }
            if ( map[ idx ] >= 0 ) {
                A_compose_clsure_updt( map[ idx ] );
                return( map[ idx ] );
            }
            assert( low_water <= idx );
            if ( low_water > idx ) {
                low_water = idx;
            }
        }

        p1  = A1-> A_p[ state1 ];
        p1z = A1-> A_p[ state1 + 1 ];
        p2  = A2-> A_p[ state2 ];
        p2z = A2-> A_p[ state2 + 1 ];
        s1 = s2 = (-1);
        while( p1 < p1z || p2 < p2z ) {
            if ( s1 < 0 ) {
                if ( p1 < p1z ) {
                    if ( A1-> A_nT == 1 ) {
                        s1 = p1-> A_b;
                        t1 = 0;
                    } else if ( A1-> A_nT == 2 ) {
                        s1 = p1-> A_b;
                        t1 = s1 & 1;
                        s1 >>= 1;
                    } else {
                        s1 = p1-> A_b / A1-> A_nT;
                        t1 = p1-> A_b - s1 * A1-> A_nT;
                    }
                } else  s1 = MAXSHORT;
            }
            if ( s2 < 0 ) {
                if ( p2 < p2z ) {
                    if ( A2-> A_nT == 1 ) {
                        s2 = p2-> A_b;
                        t2 = 0;
                    } else if ( A2-> A_nT == 2 ) {
                        s2 = p2-> A_b;
                        t2 = s2 & 1;
                        s2 >>= 1;
                    } else {
                        s2 = p2-> A_b / A2-> A_nT;
                        t2 = p2-> A_b - s2 * A2-> A_nT;
                    }
                } else  s2 = MAXSHORT;
            }
            if ( p1-> A_b == 1 && p2-> A_b == 1 ) {
                goto A_compose_clsure_exit;
            } else if ( p1-> A_b == 1 ) {
                ++p1;
                s1 = (-1);
            } else if ( p2-> A_b == 1 ) {
                ++p2;
                s2 = (-1);
            } else if ( t1 != A1-> A_nT-1 || t2 != 0 ) {
                goto A_compose_clsure_exit;
            } else if ( s1 == s2 ) {
                if ( found_one == 1 ) {
                    goto A_compose_clsure_exit;
                }
                candidate_state1 = p1-> A_c;
                candidate_state2 = p2-> A_c;
                candidate_flag   = 1;
                found_one = 1;
                ++p1;
                ++p2;
                s1 = s2 = (-1);
            } else if ( s1 <= s2 ) {
                ++p1;
                s1 = (-1);
            } else {
                ++p2;
                s2 = (-1);
            }
        }
        if ( found_one ) {
            state1 = candidate_state1;
            state2 = candidate_state2;
            flag   = candidate_flag;
            found_one = 0;
        } else {
            goto A_compose_clsure_exit;
        }
    }

A_compose_clsure_exit:
    map_value = U_insert( U, state1, state2, flag );
    A_compose_clsure_updt( map_value );
    return( map_value );
}
