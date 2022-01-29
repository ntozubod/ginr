#!/usr/bin/env perl
use warnings;
use strict;
use utf8;

open( OUT, "> :encoding(UTF-8)", "char.out" );
for( my $i = 0; $i < 256; ++$i ) {
  print OUT sprintf( "( `%02x`, %x %x ), ", $i, int( $i / 16 ), $i % 16 );
  if ( $i % 4 == 3 ) {
    print OUT "\n";
  }
}
close( OUT );
