#!/usr/bin/env perl
use 5.14.2;
use warnings;
use Test::More;
use B::OpTreeVisitor qw(:all);

ok(defined(VISIT_CONT), "VISIT_CONT constant");
ok(defined(VISIT_SKIP), "VISIT_SKIP constant");
ok(defined(VISIT_ABORT), "VISIT_ABORT constant");

my $called = 0;
sub callback {
  my ($op, $parent) = @_;
  pass("Callback called");
  ok(ref($op) =~ /^B::/, '$op is a B::$something');
  ok(ref($parent) =~ /^B::/, '$parent is a B::$something');

  $called++;
  return VISIT_CONT;
}

sub callback_abort {
  my ($op, $parent) = @_;
  $called++;
  return VISIT_ABORT;
}

my $walker = B::OpTreeVisitor->new(\&callback);
isa_ok($walker, "B::OpTreeVisitor");

sub testsub {
  my $x = 2;
}

$walker->visit_cv(\&testsub);
ok($called > 1);

$walker = B::OpTreeVisitor->new(\&callback_abort);
isa_ok($walker, "B::OpTreeVisitor");

$called = 0;
$walker->visit_cv(\&testsub);
ok($called == 1);

done_testing();
