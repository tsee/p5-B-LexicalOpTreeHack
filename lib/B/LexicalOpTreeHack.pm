package B::LexicalOpTreeHack;
use 5.12.0;
use warnings;
our $VERSION = '0.01';
# ABSTRACT: frobnicates foobles

require XSLoader;
XSLoader::load("B::LexicalOpTreeHack", $VERSION);

CHECK {
  B::LexicalOpTreeHack::global_check_hook();
}

sub enable {
  $^H{lexical_optree_hack} = $_[0];
}

1;
__END__

