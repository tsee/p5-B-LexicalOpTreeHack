#!/usr/bin/env perl

use t::lib::Test;

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

is(my_hack::called, 1);
is_deeply([sort map cvname($_), my_hack::candidates],
          [qw(__ANON__ bar foo)]);
my_hack::reset;

done_testing();
