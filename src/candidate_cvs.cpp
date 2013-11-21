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
  printf("TODO: should be churning through candidate CVs here!\n");
  LO_candidate_cvs.clear();
}
