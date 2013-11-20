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
  LO_candidate_cvs.insert(cv);
}

