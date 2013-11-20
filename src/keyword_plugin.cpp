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

    /* If in eval, emit UNITCHECK */
    if (IN_EVAL) {
      if (!PL_unitcheckav)
          PL_unitcheckav = newAV();
      av_unshift(PL_unitcheckav, 1);
      /* TODO get_cv output can be cached */
      SV *check_hook = (SV *)get_cv("B::LexicalOpTreeHack::global_check_hook", 0);
      SvREFCNT_inc(check_hook);
      if (!av_store(PL_unitcheckav, 0, check_hook))
        SvREFCNT_dec(check_hook);
    }

    ret = (*LO_next_keyword_plugin)(aTHX_ keyword_ptr, keyword_len, op_ptr);
  } else {
    ret = (*LO_next_keyword_plugin)(aTHX_ keyword_ptr, keyword_len, op_ptr);
  }

  return ret;
}
