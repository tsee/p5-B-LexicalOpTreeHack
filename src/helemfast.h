#ifndef HELEMFAST_H_
#define HELEMFAST_H_

#include <EXTERN.h>
#include <perl.h>

void hf_init_global_state(pTHX);
OP *hf_prepare_helemfast_lex(pTHX_ PADOFFSET padoffset, SV *key);


#endif
