#!/usr/bin/env perl

use t::lib::Test;
use Data::Dumper;

sub cop_hints_ok {
  my @hinted_cops = map @{ $_->hinted_cops($my_hack::HintName) }, my_hack::candidates();
  foreach my $cop (@hinted_cops) {
    is($cop->name, 'nextstate', "OP is a nextstate");
    my $hh = B::COP::hints_hash($cop)->HASH;
    # FIXME hardcoded full hint name breaks encapsulation. Just for testing!
    ok($hh->{"lexical_optree_hack/my_hack"}, "Our hint is set")
      or print Dumper $hh;
  }
}

use B::LexicalOpTreeHack qw(has_hint);

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
cop_hints_ok();
my_hack::reset;

eval "sub moo1 { }; 1" or die $@;
is(my_hack::called, 0);
cop_hints_ok();
my_hack::reset;

eval "use my_hack; sub moo2 { }; 1" or die $@;
is(my_hack::called, 1);
is_deeply([sort map cvname($_), my_hack::candidates],
          [qw(__EVAL__ moo2)]);
cop_hints_ok();
my_hack::reset;

eval "sub moo3 { use my_hack; }; 1" or die $@;
is(my_hack::called, 1);
is_deeply([sort map cvname($_), my_hack::candidates],
          [qw(moo3)]);
cop_hints_ok();
my_hack::reset;

use my_hack;
eval "sub moo4 { }; 1" or die $@;
is(my_hack::called, 1);
is_deeply([sort map cvname($_), my_hack::candidates],
          [qw(__EVAL__ moo4)]);
cop_hints_ok();
my_hack::reset;

done_testing();
