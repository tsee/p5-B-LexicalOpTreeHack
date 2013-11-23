#include "nested_sub_check.h"
#include "candidate_cvs.h"

typedef OP *(*pl_check_t) (pTHX_ OP *op);

static pl_check_t original_leavesub_check;
static pl_check_t original_leavesublv_check;

static OP *my_leavesub_check(pTHX_ OP *op);
static OP *my_leavesublv_check(pTHX_ OP *op);

void
init_nested_sub_check(pTHX)
{
  original_leavesub_check = PL_check[OP_LEAVESUB];
  PL_check[OP_LEAVESUB] = my_leavesub_check;

  original_leavesublv_check = PL_check[OP_LEAVESUBLV];
  PL_check[OP_LEAVESUBLV] = my_leavesublv_check;
}


static OP *
my_leavesub_check(pTHX_ OP *op)
{
  if (PL_compcv && PL_compiling.cop_hints_hash)
    add_candidate_cv_if_hint_enabled(aTHX_ PL_compcv);

  return original_leavesub_check(aTHX_ op);
}


static OP *
my_leavesublv_check(pTHX_ OP *op)
{
  if (PL_compcv && PL_compiling.cop_hints_hash)
    add_candidate_cv_if_hint_enabled(aTHX_ PL_compcv);

  return original_leavesublv_check(aTHX_ op);
}
