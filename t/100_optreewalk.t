#!/usr/bin/env perl
use 5.14.2;
use warnings;
use Test::More;
use B::OpTreeVisitor qw(:all);

ok(defined(VISIT_CONT), "VISIT_CONT constant");
ok(defined(VISIT_SKIP), "VISIT_SKIP constant");
ok(defined(VISIT_ABORT), "VISIT_ABORT constant");

my $called = 0;
sub callback_cont {
  my ($op, $parent) = @_;
  ok(ref($op) =~ /^B::/, '$op is a B::$something');
  ok(ref($parent) =~ /^B::/, '$parent is a B::$something');

  $called++;
  return VISIT_CONT;
}

sub callback_skip {
  my ($op, $parent) = @_;
  $called++;
  return VISIT_SKIP;
}

sub callback_abort {
  my ($op, $parent) = @_;
  $called++;
  return VISIT_ABORT;
}

sub testsub {
  my $x = 2;
  $x = 5;
  $x = 12;
}

foreach my $class (qw(B::OpTreeVisitor B::OpTreeVisitor::CopFinder)) {
  my $walker;

  # Test VISIT_CONT
  $walker = $class->new(\&callback_cont);
  isa_ok($walker, $class);

  $called = 0;
  $walker->visit_cv(\&testsub);
  ok($called > 1, "VISIT_CONT visits more than one OP ($called), $class");

  # Test VISIT_SKIP
  $walker = $class->new(\&callback_skip);
  isa_ok($walker, $class);

  my $full_called_count = $called;
  $called = 0;
  $walker->visit_cv(\&testsub);
  if ($class =~ /CopFinder/) {
    ok($called <= $full_called_count,
       "VISIT_SKIP visits less than or equal to VISIT_CONT " .
       "(VISIT_SKIP=$called, VISIT_CONT=$full_called_count), $class");
  }
  else {
    ok($called < $full_called_count,
       "VISIT_SKIP visits less than VISIT_CONT " .
       "(VISIT_SKIP=$called, VISIT_CONT=$full_called_count), $class");
  }

  # Test VISIT_ABORT
  $walker = $class->new(\&callback_abort);
  isa_ok($walker, $class);

  $called = 0;
  $walker->visit_cv(\&testsub);
  ok($called == 1, "VISIT_ABORT aborted walk successfully ($called calls), $class");
}


# Very crude, incomplete test for hint check
my $walker = B::OpTreeVisitor::CopFinder->new(\&callback_cont, "THIS HINT DOES NOT EXIST");
$called = 0;
$walker->visit_cv(\&testsub);
is($called, 0, "Non-existant hint prevents any callback from being invoked (called: $called)");

done_testing();
