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
#include "local.h"

extern FILE *fpout;

typedef struct S_f {
    unsigned    S_linkb : 26;
    unsigned    S_kval  :  5;
    unsigned    S_tag   :  1;
    struct S_f *S_linkf;
} S_ft;

#define tag(p)          ((p)-> S_tag)
#define kval(p)         ((p)-> S_kval)
#define linkf(p)        ((p)-> S_linkf)
#define linkb(p)        ((S_ft *)(p)-> S_linkb)

#define set_tag(p,t)    (p)-> S_tag = t
#define set_kval(p,k)   (p)-> S_kval = k
#define set_linkf(p,q)  (p)-> S_linkf = q
#define set_linkb(p,q)  (p)-> S_linkb = (unsigned)(q)

#define S_m             20
/* S_m = 20 allows objects of up to 4 megabytes */

static S_ft *S_lo = 0,
             *S_hi = 0,
              S_avail[S_m+1];

static int   S_alld_cnt[S_m];

#ifdef UNIX
extern char *sbrk();
#endif

#ifdef CMS
extern char *_getmain();
int    CMSmem = 0;
#endif

/*
 *     Copy a block of memory (VAX and portable versions
 */

#ifdef VAX
void copymem( n, from, to )
int n;
register char *from, *to;
{
    register i;
    if ( n > 65535L && from < to && from + n > to )
        Error( "copymem: Destructive copy" );
    while ( n > 0 ) {
        i = n > 65535L ? 65535L : n;
        asm("   movc3   r9,(r11),(r10)");
        n -= i;
        from += i;
        to += i;
    }
}
#else
void copymem( n, from, to )
register int n;
register char *from, *to;
{
    if ( from + n <= to || to + n <= from ) {
        bcopy( from, to, n );
        return;
    }
    if ( from >= to )
        while ( --n >= 0 ) *to++ = *from++;
    else {
        from += n;
        to += n;
        while ( --n >= 0 ) *--to = *--from;
    }
}
#endif

void scribble( p, q )
register char *p, *q;
{
    while( p < q ) *p++ = 0x55;
}

/*
 *     Binary Buddy system storage allocator as in Knuth vol. 1
 */

void S_init()
{
    register S_ft *p;
    register int i;
    if ( S_lo == 0 ) {
#ifdef UNIX
        (void) sbrk( -(int)sbrk(0) & 0x3ff );
        S_lo = S_hi = (S_ft *) sbrk(0);
#endif
#ifdef CMS
        for( CMSmem = 16 * 1024 * 1024; S_lo == 0; ) {
            if ( CMSmem > 4 * 1024 * 1024 )
                CMSmem -= 512 * 1024;
            else    CMSmem -= 128 * 1024;
            S_lo = S_hi = (S_ft *) _getmain( CMSmem );
        }
        fflush( fpout );
#endif
        set_linkf( p = &S_avail[ S_m ], 0 );
        while( --p >= S_avail ) {
            set_linkf( p, p );
            set_linkb( p, p );
        }
        for ( i = 0; i < S_m; ++i ) S_alld_cnt[i] = 0;
    }
}

void S_free( l, k )
register S_ft *l;
register int k;
{
    register S_ft *p;
    if ( (int) l & 7 ) Error( "S_free: l not divisible by 8" );
    if ( l < S_lo || l + (1 << k ) > S_hi ) Error( "S_free: bounds" );
    if ( l-S_lo & (1 << k)-1 ) Error( "S_free: l improper" );
    if ( tag(l) ) Error( "S_free: attempt to free unallocated block" );
    --S_alld_cnt[k];
    for( ; k < S_m - 1; ++k ) {
        p = S_lo + ( l - S_lo ^ 1 << k );
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

void S_morecore( k )
register int k;
{

    register int a, b;
    register S_ft *p;
#ifdef UNIX
    if ( k < 7 ) k = 7;
    a = S_hi - S_lo;
    p = (S_ft *)sbrk( sizeof(S_ft) * ( ( (a+(2<<k)-1) >> k << k ) - a ) );
    if ( (int) p == (-1) ) Error( "S_morecore: Out of Memory" );
    if ( p != S_hi ) Error( "S_morecore: Another allocator active" );
    a = S_hi - S_lo;
    S_hi = (S_ft *) sbrk(0);
    /*      scribble( S_lo + a, S_hi ); */
#endif
#ifdef CMS
    if ( S_hi != S_lo ) Error( "S_morecore: Out of Memory" );
    a = 0;
    S_hi = S_lo + CMSmem / sizeof(S_ft);
#endif
    b = S_hi - S_lo;
    while ( a < b ) {
        for( k = 0; !(a>>k & 1) && (b-a) >> (k+1); ++k );
        ++S_alld_cnt[k];
        set_tag( S_lo + a, 0 );
        S_free( S_lo + a, k );
        a += 1 << k;
    }
}

S_ft *S_malloc( k )
register int k;
{
    register int j;
    register S_ft *p, *l, *q;
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

S_ft *S_realloc( l, k1, k2 )
register S_ft *l;
register int k1, k2;
{
    register int k0;
    register S_ft *p, *q;
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
            p = S_lo + ( l - S_lo ^ 1 << k1 );
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
        copymem( sizeof(S_ft) << k0, l, p );
        ++S_alld_cnt[k1];
        S_free( l, k1 );
        set_kval( p, k2 );
        return( p );
    }
}

S_ft *S_copy( l, k )
register S_ft *l;
register int k;
{
    register S_ft *p;
    p = S_malloc( k );
    copymem( sizeof(S_ft) << k, l, p );
    return( p );
}

void S_arena()
{
    int grand, gran2, i, size, cnt;
    S_ft *p, *q;
    S_init();
    fprintf( fpout, "Size      Free      Allocated\n" );
    fprintf( fpout, "      Number Total Number Total\n" );
    grand = 0;
    gran2 = 0;
    for( i = 0; i < S_m; ++i ) {
        q = &S_avail[i];
        if ( (p = linkf(q)) != q || S_alld_cnt[i] ) {
            size = 8 << i;
            if ( p != q )
                for( cnt = 1; ( p = linkf(p) ) != q; ++cnt );
            else cnt = 0;
            if ( size < 1024 )      fprintf( fpout, "%4d ", size );
            else                    fprintf( fpout,
                                                 "%4dK", size / 1024);
            fprintf( fpout, "%7d", cnt );
            fprintf( fpout, "%5dK", ( cnt * size + 1023 ) / 1024 );
            fprintf( fpout, "%7d", S_alld_cnt[i] );
            fprintf( fpout,
                     "%5dK\n", (S_alld_cnt[i]*size+1023)/1024);
            grand += cnt * size;
            gran2 += S_alld_cnt[i] * size;
        }
    }
    fprintf( fpout, "            %5dK", ( grand + 1023 ) / 1024 );
    fprintf( fpout, "       %5dK\n", ( gran2 + 1023 ) / 1024 );
#ifdef CMS
    size = CMSmem;
    fprintf( fpout, "Memory Size %5dK\n", size / 1024 );
#else
    size = (int) sbrk(0);
    fprintf( fpout, "High Water Mark %5dK\n", size / 1024 );
#endif
    if ( size % 1024 != 0 )
        fprintf ( fpout, "Excess %d bytes\n", size % 1024 );
}

/*
 *     Interface to provide allocator for INR.
 *     The length code and an audit flag are stored in allocated blocks
 */

char *Salloc( n )
register int n;
{
    register char *p;
    register int k;
    S_init();
    if ( n < 0 ) Error( "Salloc: Argument constraint error" );
    n = ( n + sizeof(S_ft) + 3 ) / sizeof(S_ft);
    for( k = 0; n > ( 1 << k ); ++k );
    p = (char *)S_malloc( k );
    p[0] = 0x7f;
    p[1] = k;
    return( p + 4 );
}

void Sfree( p )
register char *p;
{
    if ( !p ) return;
    p -= 4;
    if ( p[0] != 0x7f ) Error( "Sfree: Invalid free" );
    S_free( (S_ft *)p, (int)p[1] );
}

char *Srealloc( p, n )
register char *p;
register int n;
{
    register int k;
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

char *Scopy( p )
register char *p;
{
    if ( !p ) return( 0 );
    p -= 4;
    return( (char *)S_copy( (S_ft *)p, (int)p[1] ) + 4 );
}

int Ssize( p )
char *p;
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
        if ( p-S_lo & (1 << kval(p))-1 ) {
            printf( "Block alignment error at %x\n", p );
            printf( "Block size %d Offset %x\n", kval(p), p-S_lo );
            if ( last_p )
                printf( "Last good block %x kval %d\n", last_p,
                        kval(last_p) );
            return;
        }
        if ( !tag(p) ) {
            if ( pc[0] != 0x7f ) {
                printf( "Audit anomoly in busy block at %x:\n", p );
                printf( "Size code %d\n", pc[1] );
                printf( "S_lo %x S_hi %x S_avail %x\n",
                        S_lo, S_hi, S_avail );
                if ( pc[0] != 0x7f )
                    printf( "Mask is %x\n", pc[0] & 0xff );
                if ( last_p )
                    printf( "Last good block %x kval %d\n", last_p,
                            kval(last_p) );
                return;
            }
        } else {
            k = kval(p);
            if ( k >= 20
                    || (( linkf(p) < S_lo || linkf(p) >= S_hi )
                        && ( linkf(p) < S_avail
                             || linkf(p) >= S_avail + S_m ))
                    || (( linkb(p) < S_lo || linkb(p) >= S_hi )
                        && ( linkb(p) < S_avail
                             || linkb(p) >= S_avail + S_m ))
                    || linkb(linkf(p)) != p
                    || linkf(linkb(p)) != p ) {
                printf( "Audit anomoly in free block at %x:\n", p );
                printf( "S_lo %x S_hi %x S_avail %x\n",
                        S_lo, S_hi, S_avail );
                printf( "kval %d\n", k );
                printf( "linkf %x linkb %x\n", linkf(p), linkb(p) );
                if (( linkf(p) >= S_lo && linkf(p) < S_hi )
                        || ( linkf(p) >= S_avail
                             && linkf(p) < S_avail + S_m ))
                    printf( "linkb(linkf(p)) %x\n", linkb(linkf(p)) );
                if (( linkb(p) >= S_lo && linkb(p) < S_hi )
                        || ( linkb(p) >= S_avail
                             && linkb(p) < S_avail + S_m ))
                    printf( "linkf(linkb(p)) %x\n", linkf(linkb(p)) );
                for( i = 0; i < S_m; i++ ) {
                    if ( linkf(S_avail+i) == p )
                        printf( "linkf(S_avail+i) %x\n",
                                linkf(S_avail+i) );
                    if ( linkb(S_avail+i) == p )
                        printf( "linkb(S_avail+i) %x\n",
                                linkb(S_avail+i) );
                }
                if ( last_p )
                    printf( "Last good block %x kval %d\n", last_p,
                            kval(last_p) );
                return;
            }
        }
        last_p = p;
        p = p + (1 << kval(p));
    }
}

#ifndef CMS
/*
 *     Hook for malloc.  The above allocator will fail if any other
 *     allocator is active!
 */

char *malloc( n )
int n;
{
    return( Salloc( n ) );
}

void free( p )
char *p;
{
    Sfree( p );
}

char *realloc( p, n )
char *p;
int n;
{
    return( Srealloc( p, n ) );
}
#endif
