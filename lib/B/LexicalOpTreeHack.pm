package B::LexicalOpTreeHack;
use 5.12.0;
use warnings;
our $VERSION = '0.01';
# ABSTRACT: frobnicates foobles

use Exporter 'import';

our @EXPORT_OK = qw();

require XSLoader;
XSLoader::load("B::LexicalOpTreeHack", $VERSION);

1;
__END__

