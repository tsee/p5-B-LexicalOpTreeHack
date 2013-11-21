#include "candidate_cvs.h"

#include <EXTERN.h>
#include <perl.h>
#include "ppport.h"

#include <set>
#include <string>

using namespace std;

static set<CV *> LO_candidate_cvs;

void
add_candidate_cv(pTHX_ CV *cv)
{
  // TODO for now ignore all special blocks, since most of them are
  // going to be freed early; we might still want to apply the rewrite
  // to END blocks
  if (CvSPECIAL(cv))
    return;
  LO_candidate_cvs.insert(cv);
}


static std::string
get_cv_name(pTHX_ CV *cv)
{
  if (CvGV(cv))
    return string(GvNAME(CvGV(cv)));
#if PERL_VERSION >= 18
  else if (CvNAME_HEK(*it))
    return string(CvNAME_HEK(cv)->hek_key);
#endif
  else
    return string("eval/main");
}


void
process_candidate_cvs(pTHX)
{
  typedef set<CV *>::const_iterator iterator;
  printf("TODO: should be churning through %d candidate CVs here!\n", LO_candidate_cvs.size());

  for (iterator it = LO_candidate_cvs.begin(), end = LO_candidate_cvs.end();
       it != end; ++it) {

    printf("  CV: %s\n", get_cv_name(aTHX_ *it).c_str());
  }
  LO_candidate_cvs.clear();
}


bool
is_hint_enabled(pTHX)
{
  // TODO add API to register hints
  SV *hint = cophh_fetch_pvn(PL_compiling.cop_hints_hash, "lexical_optree_hack", 19, 0, 0);

  return hint != &PL_sv_placeholder && SvTRUE(hint);
}

