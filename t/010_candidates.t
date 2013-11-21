#!/usr/bin/env perl
use 5.12.0;
use warnings;
use Test::More;

package my_hack;
BEGIN { $INC{"my_hack.pm"} = 1 }

use B::LexicalOpTreeHack;

sub import { B::LexicalOpTreeHack->enable }

package main;

sub foo {
  use my_hack;
  my $cb = sub {1};
}

pass();
done_testing();
