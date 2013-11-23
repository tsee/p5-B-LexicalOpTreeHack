#include "nested_sub_check.h"
#include "candidate_cvs.h"

typedef OP *(*pl_check_t) (pTHX_ OP *op);

static pl_check_t original_leavesub_check;
static pl_check_t original_leavesublv_check;
static pl_check_t original_leaveeval_check;

static OP *my_leavesub_check(pTHX_ OP *op);
static OP *my_leavesublv_check(pTHX_ OP *op);
static OP *my_leaveeval_check(pTHX_ OP *op);

void
init_nested_sub_check(pTHX)
{
  original_leavesub_check = PL_check[OP_LEAVESUB];
  PL_check[OP_LEAVESUB] = my_leavesub_check;

  original_leavesublv_check = PL_check[OP_LEAVESUBLV];
  PL_check[OP_LEAVESUBLV] = my_leavesublv_check;

  original_leaveeval_check = PL_check[OP_LEAVEEVAL];
  PL_check[OP_LEAVEEVAL] = my_leaveeval_check;
}


static OP *
my_leavesub_check(pTHX_ OP *op)
{
  if (PL_compcv && PL_compiling.cop_hints_hash)
    add_candidate_cv_if_hint_enabled(aTHX_ PL_compcv, false);

  return original_leavesub_check(aTHX_ op);
}


static OP *
my_leavesublv_check(pTHX_ OP *op)
{
  if (PL_compcv && PL_compiling.cop_hints_hash)
    add_candidate_cv_if_hint_enabled(aTHX_ PL_compcv, false);

  return original_leavesublv_check(aTHX_ op);
}


static OP *
my_leaveeval_check(pTHX_ OP *op)
{
  // we don't check PL_compiling.cop_hints_hash because we want to
  // install the PL_unitcheckav hook if there is a subroutine with
  // hints, even if the eval top level does not have hints
  if (PL_compcv)
    add_candidate_cv_if_hint_enabled(aTHX_ PL_compcv, true);

  return original_leaveeval_check(aTHX_ op);
}
