package nextstates;

use 5.14.0;
use warnings;

use B::LexicalOpTreeHack qw(has_hint);
use B::Generate;

use constant HINT => 'no_nextstates';

BEGIN { B::LexicalOpTreeHack::register(HINT, \&_kill_nextstates) }

# the pragma is enabled by 'no nextsates'
sub import { B::LexicalOpTreeHack::enable(HINT, 0) }
sub unimport { B::LexicalOpTreeHack::enable(HINT, 1) }

sub _kill_nextstates {
  for my $code (@_) {
    for (my ($curr, $pred) = $code->start; $$curr; $pred = $curr, $curr = $curr->next) {
      if ($curr->name eq 'nextstate' && $pred && has_hint($curr, HINT)) {
        # do the easy thing and just patch the nextstate out of execution order
        $pred->next($curr->next);
      }
    }
  }
}

1;
