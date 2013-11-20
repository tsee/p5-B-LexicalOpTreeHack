#ifndef CANDIDATE_CVS_H_
#define CANDIDATE_CVS_H_

#include <EXTERN.h>
#include <perl.h>

void add_candidate_cv(pTHX_ CV *cv);

void process_candidate_cvs(pTHX);


#endif
