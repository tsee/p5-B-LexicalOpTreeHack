#!/usr/bin/env perl
use 5.14.2;
use warnings;
use lib 't/lib';
use Test::More;

sub foo {
  use helemfast;
  my %h = (bar => "baz");
  my $x;
  for (1..1000000) {
    $x = $h{bar};
  }
  return $x;
}

is(foo(), "baz");

pass("Alive");
done_testing();
