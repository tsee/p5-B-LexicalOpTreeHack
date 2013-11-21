#include "candidate_cvs.h"

#include <EXTERN.h>
#include <perl.h>
#include "ppport.h"

#include <set>

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


void
process_candidate_cvs(pTHX)
{
  typedef set<CV *>::const_iterator iterator;
  printf("TODO: should be churning through %d candidate CVs here!\n", LO_candidate_cvs.size());

  for (iterator it = LO_candidate_cvs.begin(), end = LO_candidate_cvs.end();
       it != end; ++it) {
    if (CvGV(*it))
      printf("  CV: %s\n", GvNAME(CvGV(*it)));
#if PERL_VERSION >= 18
    else if (CvNAME_HEK(*it))
      printf("  CV: %s\n", CvNAME_HEK(*it)->hek_key);
#endif
    else
      printf("  eval/main\n");
  }
  LO_candidate_cvs.clear();
}
