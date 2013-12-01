package B::OpTreeVisitor;
use 5.12.0;
use warnings;
our $VERSION = '0.01';
# ABSTRACT: Walks the OP tree and calls your Perl function on each OP

use B::LexicalOpTreeHack;
use Exporter 'import';

use constant VISIT_CONT  => 0;
use constant VISIT_SKIP  => 1;
use constant VISIT_ABORT => 2;

our @EXPORT_OK = qw(VISIT_CONT VISIT_SKIP VISIT_ABORT);
our %EXPORT_TAGS = ("all" => \@EXPORT_OK);

1;
__END__

