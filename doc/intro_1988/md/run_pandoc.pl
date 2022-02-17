#!/usr/bin/env perl
use warnings;
use strict;
use utf8;

my $s1 =
   '<img src="https://render.githubusercontent.com/render/math?math=';
my $s2 = '">';

system( "cat ../latex/0*.tex > a.tex" );

open( AA, "< :encoding(UTF-8)", "a.tex" );
open( BB, "> :encoding(UTF-8)", "b.tex" );
my $buf = '';
while ( my $aa = <AA> ) {
  $aa =~ s/\s*$//;
  my $bb = $aa;
  $buf .= $bb . "\n";
}
$buf =~ s/(\\verb\#)([0-9])/$1\\=$2/g;
print BB $buf;
close( AA );
close( BB );

system( "pandoc -f latex -t markdown < b.tex > c.md" );

open( AA, "< :encoding(UTF-8)", "c.md" );
open( BB, "> :encoding(UTF-8)", "d.md" );
$buf = '';
while ( my $aa = <AA> ) {
  $aa =~ s/\s*$//;
  my $bb = $aa;
  $bb =~ s/\$([a-z])\$/*$1*/g;
  $bb =~ s/\$([+-])\$/$1/g;
  if ( ! ( $bb =~ /`\$`/ ) ) {
    while ( $bb =~ s/\s*\$\$([^\$]+)\$\$\s*/\n\n> [[[$1]]]\n\n/g  ) { }
    while ( $bb =~ s/\$([^\$]+)\$/[[[$1]]]/g  ) { }
  }
  $bb =~ s/\[\[\[0\)-\(R\]\]\]/\$0)-R\$/;
  $buf .= $bb . "\n";
}
$buf =~ s/\\=([0-9])/$1/g;
print BB $buf;
close( AA );
close( BB );

open( AA, "< :encoding(UTF-8)", "d.md" );
open( BB, "> :encoding(UTF-8)", "../inr_intro.md" );
$buf = '';
while ( my $aa = <AA> ) {
  $aa =~ s/\s*$//;
  my $bb = $aa;
  while ( $bb =~ s/\[\[\[([^\]]+)\]\]\]/$s1 . $1 . $s2/eg  ) { }
  $buf .= $bb . "\n";
}
$buf =~ s/\n\n\n+/\n\n/g;
print BB $buf;
close( AA );
close( BB );
