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
          [qw(__ANON__ __MAIN__ bar foo)]);
my_hack::reset;

eval "sub moo1 { }; 1" or die $@;
is(my_hack::called, 0);
my_hack::reset;

eval "use my_hack; sub moo2 { }; 1" or die $@;
is(my_hack::called, 1);
is_deeply([sort map cvname($_), my_hack::candidates],
          [qw(__MAIN__ moo2)]);
my_hack::reset;

eval "sub moo3 { use my_hack; }; 1" or die $@;
is(my_hack::called, 1);
is_deeply([sort map cvname($_), my_hack::candidates],
          [qw(moo3)]);
my_hack::reset;

use my_hack;
eval "sub moo4 { }; 1" or die $@;
is(my_hack::called, 1);
is_deeply([sort map cvname($_), my_hack::candidates],
          [qw(__MAIN__ moo4)]);
my_hack::reset;

done_testing();
