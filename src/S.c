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
#include <stdlib.h>
#include <strings.h>
#include "local.h"
#include "O.h"

extern FILE *fpout;

typedef struct S_f {
    unsigned char fill_1;
    unsigned char fill_2;
    unsigned char S_kval;
    unsigned char S_tag;
    unsigned      fill_3;
    struct S_f *fill_4;
    struct S_f *S_linkf;
    struct S_f *S_linkb;
} S_ft;

#define tag(p)          ((p)-> S_tag)
#define kval(p)         ((p)-> S_kval)
#define linkf(p)        ((p)-> S_linkf)
#define linkb(p)        ((p)-> S_linkb)

#define set_tag(p,t)    (p)-> S_tag = t
#define set_kval(p,k)   (p)-> S_kval = k
#define set_linkf(p,q)  (p)-> S_linkf = q
#define set_linkb(p,q)  (p)-> S_linkb = q

#define U(p)            ((unsigned long)(p))

#define S_m             28
/* S_m = 26 allows objects of up to 2 gigabytes 2^(5+26)*/
/* S_m = 28 allows objects of up to 8 gigabytes 2^(5+28)*/

static S_ft *S_lo = 0,
             *S_hi = 0,
              S_avail[S_m+1];

static int   S_alld_cnt[S_m];

long   LINUXmem = 0;

/*
 *     Copy a block of memory
 */

void copymem( long n, char *from, char *to )
{
    if ( from + n <= to || to + n <= from ) {
        bcopy( from, to, n );
        return;
    }
    if ( from >= to ) {
        while ( --n >= 0 ) {
            *to++ = *from++;
        }
    } else {
        from += n;
        to += n;
        while ( --n >= 0 ) *--to = *--from;
    }
}

void scribble( char *p, char *q )
{
    while( p < q ) *p++ = 0x55;
}

/*
 *     Binary Buddy system storage allocator as in Knuth vol. 1
 */

void S_init()
{
    S_ft *p;
    int i;
    if ( S_lo == 0 ) {
        long mem;
        for( mem = 512 * 1024 * 1024; S_lo == 0; mem /= 2 ) {
            LINUXmem = mem;
            S_lo = S_hi = (S_ft *) malloc( mem + 16 );
        }
        fflush( fpout );
        set_linkf( p = &S_avail[ S_m ], 0 );
        while( --p >= S_avail ) {
            set_linkf( p, p );
            set_linkb( p, p );
        }
        for ( i = 0; i < S_m; ++i ) S_alld_cnt[i] = 0;
    }
}

void S_free( S_ft *l, int k )
{
    S_ft *p;
    if ( (long) l & 7 ) Error( "S_free: l not divisible by 8" );
    if ( l < S_lo || l + (1 << k) > S_hi ) Error( "S_free: bounds" );
    if ( ( l-S_lo ) & ((1 << k)-1) ) Error( "S_free: l improper" );
    if ( tag(l) ) Error( "S_free: attempt to free unallocated block" );
    --S_alld_cnt[k];
    for( ; k < S_m - 1; ++k ) {
        p = S_lo + ( ( l - S_lo ) ^ ( 1 << k ) );
        if ( p >= S_hi || !tag(p) || kval(p) != k ) break;
        set_linkf( linkb(p), linkf(p) );
        set_linkb( linkf(p), linkb(p) );
        if ( p < l ) l = p;
    }
    set_tag( l, 1 );
    set_kval( l, k );
    set_linkf( l, p = linkf( &S_avail[k] ) );
    set_linkb( p, l );
    set_linkb( l, p = &S_avail[k] );
    set_linkf( p, l );
}

void S_morecore( int k )
{

    long a, b;
    if ( S_hi != S_lo ) Error( "S_morecore: Out of Memory" );
    a = 0;
    S_hi = S_lo + LINUXmem / sizeof(S_ft);
    b = S_hi - S_lo;
    while ( a < b ) {
        for( k = 0; !(a>>k & 1) && (b-a) >> (k+1); ++k );
        ++S_alld_cnt[k];
        set_tag( S_lo + a, 0 );
        S_free( S_lo + a, k );
        a += 1 << k;
    }
}

S_ft *S_malloc( int k )
{
    int j;
    S_ft *p, *l, *q;
    if ( k >= S_m ) Error( "S_malloc: Argument constraint error" );
    ++S_alld_cnt[k];
    for(;;) {
        for( j = k; linkf( &S_avail[j] ) == &S_avail[j]; ++j );
        if ( j < S_m ) break;
        S_morecore( k );
    }
    l = linkf( p = &S_avail[j] );
    set_linkf( p, linkf(l) );
    set_linkb( linkf(l), p );
    set_tag( l, 0 );
    set_kval( l, k );
    set_linkf( l, 0 );
    set_linkb( l, 0 );
    while ( --j >= k ) {
        p = l + ( 1 << j );
        set_tag( p, 1 );
        set_kval( p, j );
        set_linkf( p, q = &S_avail[j] );
        set_linkb( p, q );
        set_linkf( q, p );
        set_linkb( q, p );
    }
    return( l );
}

S_ft *S_realloc( S_ft *l, int k1, int k2 )
{
    int k0;
    S_ft *p, *q;
    --S_alld_cnt[k1];
    ++S_alld_cnt[k2];
    if ( k1 >= k2 ) {
        while ( --k1 >= k2 ) {
            p = l + ( 1 << k1 );
            set_tag( p, 1 );
            set_kval( p, k1 );
            set_linkb( p, q = linkb( &S_avail[k1] ) );
            set_linkf( q, p );
            set_linkf( p, q = &S_avail[k1] );
            set_linkb( q, p );
        }
        set_kval( l, k2 );
        return( l );
    } else {
        k0 = k1;
        for( ; k1 < k2; ++k1 ) {
            p = S_lo + ( ( l - S_lo ) ^ ( 1 << k1 ) );
            if ( p < l || p >= S_hi || !tag(p)
                    || kval(p) != k1 ) break;
            set_linkf( linkb(p), linkf(p) );
            set_linkb( linkf(p), linkb(p) );
        }
        if ( k1 == k2 ) {
            set_kval( l, k2 );
            return( l );
        }
        --S_alld_cnt[k2];
        p = S_malloc( k2 );
        copymem( sizeof(S_ft) << k0, ( char * ) l, ( char * ) p );
        ++S_alld_cnt[k1];
        S_free( l, k1 );
        set_kval( p, k2 );
        return( p );
    }
}

S_ft *S_copy( S_ft *l, int k )
{
    S_ft *p;
    p = S_malloc( k );
    copymem( sizeof(S_ft) << k, ( char * ) l, ( char * ) p );
    return( p );
}

void S_arena()
{
    int i, cnt;
    long grand, gran2, size;
    S_ft *p, *q;
    S_init();
    fprintf( fpout, "Size      Free      Allocated\n" );
    fprintf( fpout, "      Number Total Number Total\n" );
    grand = 0;
    gran2 = 0;
    for( i = 0; i < S_m; ++i ) {
        q = &S_avail[i];
        if ( (p = linkf(q)) != q || S_alld_cnt[i] ) {
            size = sizeof( S_ft ) << i;
            if ( p != q )
                for( cnt = 1; ( p = linkf(p) ) != q; ++cnt );
            else cnt = 0;
            if ( size < 1024 )      fprintf( fpout, "%4ld ", size );
            else if ( size < 1024 * 1024 )
                                    fprintf( fpout,
                                                 "%4ldK", size / 1024);
            else                    fprintf( fpout,
                                                 "%4ldM", size / 1024 / 1024 );
            fprintf( fpout, "%7d", cnt );
            fprintf( fpout, "%5ldM", ( cnt * size + 1023 ) / 1024 / 1024 );
            fprintf( fpout, "%7d", S_alld_cnt[i] );
            fprintf( fpout,
                     "%5ldM\n", (S_alld_cnt[i]*size+1023)/1024/1024);
            grand += cnt * size;
            gran2 += S_alld_cnt[i] * size;
        }
    }
    fprintf( fpout, "            %5ldM", ( grand + 1023 ) / 1024 / 1024 );
    fprintf( fpout, "       %5ldM\n", ( gran2 + 1023 ) / 1024 / 1024 );
    size = LINUXmem;
    fprintf( fpout, "Memory Size %5ldM\n", size / 1024 / 1024 );
    if ( size % 1024 != 0 )
        fprintf ( fpout, "Excess %ld bytes\n", size % 1024 );
}

// Check this out !!!!! JHJ
// Find the block that contains the provided address

S_ft *S_find( char *p )
{
    if ( p < (char *) S_lo || p >= (char *) S_hi ) {
        Error( "S_find: BOTCH 1" );
    }

    S_ft *l = S_lo;
    long incr  = ( p - (char *) S_lo ) / sizeof(S_ft);
    long offset = ( p - (char *) S_lo ) % sizeof(S_ft);

    if ( (char * ) &l[ incr ] + offset != p ) {
        Error( "S_find: BOTCH 2" );
    }

    if ( offset < 0 || offset >= sizeof(S_ft) ) {
        Error( "S_find: BOTCH 3" );
    }

    long base = 0;
    int base_k = kval( &l[ base ] );

    long right = S_hi - S_lo;
    int k;

    for (   k = 0;
            ( incr >> k );
            ++k ) {
        ;
    }
    --k;
    right = ( 1 << k );

// printf( "\n" );
// printf( "base %ld\n", base );
// printf( "incr %ld\n", incr );
// printf( "k %d\n", k );
// printf( "base_k %d\n", base_k );
// printf( "right %ld\n", right );

    while ( base_k <= k ) {
        base += right;
        incr -= right;
        base_k = kval( &l[ base ] );
        for (   k = 0;
                ( incr >> k );
                ++k ) {
            ;
        }
        --k;
        right = ( 1 << k );

// printf( "\n" );
// printf( "base %ld\n", base );
// printf( "incr %ld\n", incr );
// printf( "k %d\n", k );
// printf( "base_k %d\n", base_k );
// printf( "right %ld\n", right );

    }
    return ( &l[ base ] );
}

/*
 *     Interface to provide allocator for INR.
 *     The length code and an audit flag are stored in allocated blocks
 */

char *Salloc( long n )
{
    char *p;
    int k;
    S_init();
    if ( n < 0 ) Error( "Salloc: Argument constraint error" );
    n = ( n + sizeof(S_ft) + 3 ) / sizeof(S_ft);
    for( k = 0; n > ( 1 << k ); ++k );
    p = (char *)S_malloc( k );
    p[0] = 0x7f;
    p[1] = k;
    return( p + 4 );
}

void Sfree( char *p )
{
    if ( !p ) return;
    p -= 4;
    if ( p[0] != 0x7f ) Error( "Sfree: Invalid free" );
    S_free( (S_ft *)p, (int)p[1] );
}

char *Srealloc( char *p, long n )
{
    int k;
    if ( n < 0 ) Error( "Srealloc: Argument constraint error" );
    if ( !p ) return( Salloc( n ) );
    n = ( n + sizeof(S_ft) + 3 ) / sizeof(S_ft);
    for( k = 0; n > ( 1 << k ); ++k );
    p -= 4;
    p = (char *)S_realloc( (S_ft *)p, (int)p[1], k );
    p[0] = 0x7f;
    p[1] = k;
    return( p + 4 );
}

char *Scopy( char *p )
{
    if ( !p ) return( 0 );
    p -= 4;
    return( (char *)S_copy( (S_ft *)p, (int)p[1] ) + 4 );
}

long Ssize( char *p )
{
    return( ( sizeof(S_ft) << p[-3] ) - 4 );
}

void Sarena()
{
    S_arena();
}

void Saudit()
{
    S_ft *p, *last_p;
    char *pc;
    int i, k;

    printf( "<<< Audit >>>\n" );
    last_p = 0;
    for( p = S_lo; p < S_hi; ) {
        pc = (char *) p;
        if ( ( p-S_lo ) & ((1 << kval(p))-1) ) {
            printf( "Block alignment error at %lx\n", U(p) );
            printf( "Block size %d Offset %lx\n", kval(p), U(p-S_lo) );
            if ( last_p )
                printf( "Last good block %lx kval %d\n", U(last_p),
                        kval(last_p) );
            return;
        }
        if ( !tag(p) ) {
            if ( pc[0] != 0x7f ) {
                printf( "Audit anomaly in busy block at %lx:\n", U(p) );
                printf( "Size code %d\n", pc[1] );
                printf( "S_lo %lx S_hi %lx S_avail %lx\n",
                        U(S_lo), U(S_hi), U(S_avail) );
                if ( pc[0] != 0x7f )
                    printf( "Mask is %lx\n", U( pc[0] & 0xff ) );
                if ( last_p )
                    printf( "Last good block %lx kval %d\n", U(last_p),
                            kval(last_p) );
                return;
            }
        } else {
            k = kval(p);
            if ( k >= 30
                    || (( linkf(p) < S_lo || linkf(p) >= S_hi )
                        && ( linkf(p) < S_avail
                             || linkf(p) >= S_avail + S_m ))
                    || (( linkb(p) < S_lo || linkb(p) >= S_hi )
                        && ( linkb(p) < S_avail
                             || linkb(p) >= S_avail + S_m ))
                    || linkb(linkf(p)) != p
                    || linkf(linkb(p)) != p ) {
                printf( "Audit anomaly in free block at %lx:\n", U(p) );
                printf( "S_lo %lx S_hi %lx S_avail %lx\n",
                        U(S_lo), U(S_hi), U(S_avail) );
                printf( "kval %d\n", k );
                printf( "linkf %lx linkb %lx\n", U(linkf(p)), U(linkb(p)) );
                if (( linkf(p) >= S_lo && linkf(p) < S_hi )
                        || ( linkf(p) >= S_avail
                             && linkf(p) < S_avail + S_m ))
                    printf( "linkb(linkf(p)) %lx\n", U(linkb(linkf(p))) );
                if (( linkb(p) >= S_lo && linkb(p) < S_hi )
                        || ( linkb(p) >= S_avail
                             && linkb(p) < S_avail + S_m ))
                    printf( "linkf(linkb(p)) %lx\n", U(linkf(linkb(p))) );
                for( i = 0; i < S_m; i++ ) {
                    if ( linkf(S_avail+i) == p )
                        printf( "linkf(S_avail+i) %lx\n",
                                U(linkf(S_avail+i)) );
                    if ( linkb(S_avail+i) == p )
                        printf( "linkb(S_avail+i) %lx\n",
                                U(linkb(S_avail+i)) );
                }
                if ( last_p )
                    printf( "Last good block %lx kval %d\n", U(last_p),
                            kval(last_p) );
                return;
            }
        }
        last_p = p;
        p = p + (1 << kval(p));
    }
}
