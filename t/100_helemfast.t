#!/usr/bin/env perl
use 5.14.2;
use warnings;
use lib 't/lib';
use Test::More;

sub foo {
  use helemfast;
  my %h = (1..2);
  my $x = $h{1};
  return $x;
}

is(foo(), 2);

pass("Alive");
done_testing();
