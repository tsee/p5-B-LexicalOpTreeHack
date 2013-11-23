package my_hack;

use B::LexicalOpTreeHack;

my @candidates;
my $called = 0;

BEGIN { B::LexicalOpTreeHack::register("my_hack", sub {
  ++$called;
  @candidates = @_;
})};

sub import { B::LexicalOpTreeHack::enable("my_hack", 1) }
sub unimport { B::LexicalOpTreeHack::enable("my_hack", 0) }

sub called {
  return $called;
}

sub candidates {
  return @candidates;
}

sub reset {
  $called = 0;
  @candidates = ();
}

1;
