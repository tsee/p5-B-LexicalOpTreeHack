package helemfast;

use 5.14.0;
use warnings;

use B::LexicalOpTreeHack qw(has_hint);
use B::Generate;
use B::Replace qw(detach_tree replace_tree replace_op);

use constant HINT => 'helemfast';

BEGIN { B::LexicalOpTreeHack::register(HINT, \&_replace_helem) }

# the pragma is enabled by 'use helemfast'
sub import { B::LexicalOpTreeHack::enable(HINT, 1) }
sub unimport { B::LexicalOpTreeHack::enable(HINT, 0) }

sub _replace_helem {
  foreach my $code (@_) {
    foreach my $cop (@{ $code->hinted_cops(HINT) }) {
      # FIXME properly walk OP tree to find helems!
      if ($cop->name eq 'nextstate' && has_hint($cop, HINT)) { # technically superfluous at this point
        # Hardcoded test case
        my $sassign = $cop->sibling;
        next if not $sassign or $sassign->name ne 'sassign';
        my $helem = $sassign->first;
        use Devel::Peek;
        my $helemfast;
        my $keysv = $helem->last->sv;
        if (ref($keysv)) { # B::Special => really a NULL => ithreads
          $helemfast = helemfast::prepare_helemfast_lex_padkey($helem->first->targ, $helem->last->targ);
        }
        else {
          $helemfast = helemfast::prepare_helemfast_lex($helem->first->targ, $helem->last->sv);
        }
        warn "# alive";
        replace_tree($code->cv, $helem, $helemfast);
      }
    }
  }
}

1;
