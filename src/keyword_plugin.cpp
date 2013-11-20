#include "keyword_plugin.h"

#include <EXTERN.h>
#include <perl.h>
#include "ppport.h"

#include "candidate_cvs.h"

/* For chaining the actual keyword plugin */
int (*LO_next_keyword_plugin)(pTHX_ char *, STRLEN, OP **);

static int my_keyword_plugin(pTHX_ char *keyword_ptr, STRLEN keyword_len, OP **op_ptr);

#define IN_EVAL (PL_in_eval & ~(EVAL_INREQUIRE))

void
init_keyword_plugin(pTHX)
{
  LO_next_keyword_plugin = PL_keyword_plugin;
  PL_keyword_plugin = my_keyword_plugin;
}

static int
my_keyword_plugin(pTHX_ char *keyword_ptr, STRLEN keyword_len, OP **op_ptr)
{
  int ret;

  if (keyword_len == 3 && memcmp(keyword_ptr, "use", 3) == 0) {
    if (PL_compcv) /* FIXME better check for validity? */
      add_candidate_cv(aTHX_ PL_compcv);

    if (IN_EVAL) {
      /* TODO if in eval, emit UNITCHECK */
    }

    ret = (*LO_next_keyword_plugin)(aTHX_ keyword_ptr, keyword_len, op_ptr);
  } else {
    ret = (*LO_next_keyword_plugin)(aTHX_ keyword_ptr, keyword_len, op_ptr);
  }

  return ret;
}
