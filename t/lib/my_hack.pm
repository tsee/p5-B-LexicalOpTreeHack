package my_hack;

use B::LexicalOpTreeHack;

our $HintName;
BEGIN { $HintName = "my_hack"; }

my @candidates;
my $called = 0;

BEGIN { B::LexicalOpTreeHack::register($HintName, sub {
  ++$called;
  @candidates = @_;
})};

sub import { B::LexicalOpTreeHack::enable($HintName, 1) }
sub unimport { B::LexicalOpTreeHack::enable($HintName, 0) }

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
