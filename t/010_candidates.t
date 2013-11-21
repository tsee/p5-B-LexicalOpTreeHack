#!/usr/bin/env perl
use 5.12.0;
use warnings;
use Test::More;

package my_hack;
BEGIN { $INC{"my_hack.pm"} = 1 }

use B::LexicalOpTreeHack;

BEGIN { B::LexicalOpTreeHack::register("my_hack", sub { print "Got: @_\n" }) };

sub import { B::LexicalOpTreeHack::enable("my_hack", 1) }
sub unimport { B::LexicalOpTreeHack::enable("my_hack", 0) }

package main;

sub foo {
  use my_hack;
  my $cb = sub {1};
}

sub bar {
  use my_hack;
  no my_hack;
  my $cb = sub {1};
}

sub baz {
  no my_hack;
  my $cb = sub {1};
}

pass();
done_testing();
