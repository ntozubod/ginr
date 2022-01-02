#!/usr/bin/env perl
use warnings;
use strict;
use utf8;

my @files = ( split( " ",
" AGMsseq.c ALMsseq.c Abasic.c Aclos.c Aclsseq.c Acompose.c Acrea.c Aenum.c \
  Ajoin.c Alamcm.c Alameq.c Alenmin.c Aload.c Amin.c Aopen.c Apowers.c \
  Aretape.c Asseq.c Astems.c Asubs.c Atrim.c Colon.c Lex.c O.h Parse.y \
  R.c S.c T.c U.c V.c local.h localc.h localh.h locals.h \
  stamp.c util.c \
" ) );

foreach my $file ( @files ) {
  open( AA, "< :encoding(UTF-8)", $file );
  open( BB, "> :encoding(UTF-8)", "$file.new" );
  while ( my $aa = <AA> ) {
    $aa =~ s/\r?\n?$//;
    my $bb = $aa;

    if ( $aa eq
" * Not for sale or distribution without written permission of the author."
    ) {
      $bb =
" *
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
 *   along with INR.  If not, see <http://www.gnu.org/licenses/>.";
    }

    print BB $bb, "\n";
  }
  close( AA );
  close( BB );
  system( "mv $file.new $file" );
}

