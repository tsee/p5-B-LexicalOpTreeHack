package t::lib::Test;

use strict;
use warnings;
use parent 'Test::Builder::Module';

use Test::More;
use B;

require feature;

our @EXPORT = (
  @Test::More::EXPORT,
  qw(cvname),
);

sub import {
    unshift @INC, 't/lib';

    strict->import;
    warnings->import;
    feature->import(':5.12');

    goto &Test::Builder::Module::import;
}

sub cvname {
    if ($_[0]->type == B::LexicalOpTreeHack::hinted_sub()) {
        return B::svref_2object($_[0]->cv)->GV->NAME;
    } elsif ($_[0]->type == B::LexicalOpTreeHack::hinted_eval()) {
        return '__EVAL__';
    } else {
        return '__MAIN__';
    }
}

1;
