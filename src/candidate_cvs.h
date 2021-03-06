#ifndef CANDIDATE_CVS_H_
#define CANDIDATE_CVS_H_

#include <EXTERN.h>
#include <perl.h>
#include <string>

extern const std::string LO_prefix;

void init_candidate_structures(pTHX);

void register_hint(pTHX_ const char *hint, SV *callback);
void enable_hint(pTHX_ const char *hint, bool enable);
void add_candidate_cv_if_hint_enabled(pTHX_ CV *cv, bool is_eval);

bool cop_has_hint(pTHX_ COP *cop, const char *hint_name);

void process_candidate_cvs(pTHX);

#endif
