package t::lib::Test;

use strict;
use warnings;
use parent 'Test::Builder::Module';

use Test::More;
use B;

require feature;

our @EXPORT = (
  @Test::More::EXPORT,
  qw(cvname)
);

sub import {
    unshift @INC, 't/lib';

    strict->import;
    warnings->import;
    feature->import(':5.12');

    goto &Test::Builder::Module::import;
}

sub cvname {
  my $gv = B::svref_2object($_[0])->GV;

  return '__MAIN__' if $gv->isa('B::SPECIAL');
  return $gv->NAME;
}

1;
