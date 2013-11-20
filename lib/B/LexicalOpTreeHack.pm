package B::LexicalOpTreeHack;
use strict;
use warnings;
our $VERSION = '0.01';
# ABSTRACT: frobnicates foobles

require XSLoader;
XSLoader::load("B::LexicalOpTreeHack", $VERSION);

CHECK {
  B::LexicalOpTreeHack::global_check_hook();
}

1;
__END__

