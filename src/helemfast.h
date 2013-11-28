#ifndef HELEMFAST_H_
#define HELEMFAST_H_

#include <EXTERN.h>
#include <perl.h>

void hf_init_global_state(pTHX);
OP *hf_prepare_helemfast_lex(pTHX_ PADOFFSET hash_padoffset, SV *key);
OP *hf_prepare_helemfast_lex_padkey(pTHX_ PADOFFSET hash_padoffset, PADOFFSET key_padoffset);


#endif
