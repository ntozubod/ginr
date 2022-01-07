#!/usr/bin/env perl
use warnings;
use strict;
use utf8;

foreach my $arg ( @ARGV ) {
  my $targ = $arg;
  $arg =~ s/$/.orig/g;
  system( "mv $targ $arg" );
  open( AA, "< :encoding(UTF-8)", $arg );
  open( BB, "> :encoding(UTF-8)", $targ );
  my $buf = '';
  while ( my $aa = <AA> ) {
    $aa =~ s/\s*$//;
    my $bb = $aa;
    $buf .= $bb . "\n";
  }

  my @toks = ();
  while ( $buf ) {
    my $next;

    if ( $buf =~ s{^([\t ]*/\*)}{} ) {
      $next = "\n" . $1;
      if ( $buf =~ s{(^.*?\*/)[\n]?}{}s ) {
        $next .= $1 . "\n";
        push( @toks, $next );
      }
      else {
        print "Illegal block comment\n";
        push( @toks, $next . "(((IBC)))" );
      }
    }

    elsif ( $buf =~ s{^[\t ]*(//[^\n]*)[\n]?}{} ) {
      $next .= "\n" . $1 . "\n";
      push( @toks, $next );
    }

    elsif ( $buf =~ s{^[\t ]*([#][^\n]*)[\n]?}{} ) {
      $next .= $1 . "\n";
      push( @toks, $next );
    }

#  /(["'])((\\{2})*|(.*?[^\\](\\{2})*))\1/

    elsif ( $buf =~ s{^(["])}{} ) {
      $next = $1;
      my $p1 = "";
      my $p2 = "";
      my $p3 = "";
      if ( $buf =~ s{^(.*?)([\\]*)(["])}{}s ) {
        $p1 = $1;
        $p2 = $2;
        $p3 = $3;
        while ( length( $p2 ) % 2 == 1 ) {
          $next .= $p1 . $p2 . $p3;
          if ( $buf =~ s{^(.*?)([\\]*)(["])}{}s ) {
            $p1 = $1;
            $p2 = $2;
            $p3 = $3;
          }
          else {
            print "Illegal double quoted string 2\n";
            $p1 = "(((IDQS2)))";
            $p2 = "";
            $p3 = "";
          }
        }
        $next .= $p1 . $p2 . $p3;
      }
      else {
        print "Illegal double quoted string 1\n";
        $next .= "(((IDQS1)))";
      }
      push( @toks, $next );
    }

    elsif ( $buf =~ s{^(['])}{} ) {
      $next = $1;
      my $p1 = "";
      my $p2 = "";
      my $p3 = "";
      if ( $buf =~ s{^(.*?)([\\]*)(['])}{}s ) {
        $p1 = $1;
        $p2 = $2;
        $p3 = $3;
        while ( length( $p2 ) % 2 == 1 ) {
          $next .= $p1 . $p2 . $p3;
          if ( $buf =~ s{^(.*?)([\\]*)(['])}{}s ) {
            $p1 = $1;
            $p2 = $2;
            $p3 = $3;
          }
          else {
            print "Illegal single quoted string 2\n";
            $p1 = "(((ISQS2)))";
            $p2 = "";
            $p3 = "";
          }
        }
        $next .= $p1 . $p2 . $p3;
      }
      else {
        print "Illegal single quoted string 1\n";
        $next .= "(((ISQS1)))";
      }
      push( @toks, $next );
    }

    elsif ( $buf =~ s/^([ \t]+)// ) {
    }

    elsif ( $buf =~ s/^([a-zA-Z0-9_]+)// ) {
      my $next = $1;
      if ( $next ne "register" ) {
        push( @toks, $next );
      }
    }

    elsif ( $buf =~ s{^([-<>=+*/&|%^!]+)}{} ) {
      my $next = $1;
      push( @toks, $next );
    }

    elsif ( $buf =~ s{^([;{}])}{} ) {
      my $next = $1 . "\n";
      push( @toks, $next );
    }

    elsif ( $buf =~ s/^(.)//s ) {
      my $next = $1;
      if ( $next ne "\n" ) {
        push( @toks, $next );
      }
    }

  }
  $buf = join( " ", @toks );
  $buf =~ s/[ ]+\n/\n/g;
  $buf =~ s/[\n]+/\n/g;
  $buf =~ s/^\n//;

  print BB $buf;
  close( AA );
  close( BB );
}
