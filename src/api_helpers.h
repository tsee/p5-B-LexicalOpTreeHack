#ifndef API_HELPERS_H_
#define API_HELPERS_H_

#include <EXTERN.h>
#include <perl.h>

void define_constants(pTHX);

enum HintedCodeType
{
  hinted_sub,
  hinted_eval,
  hinted_main
};

struct HintedCode
{
  CV *cv;
  OP *root;
  OP *start;
  HintedCodeType type;
};

#endif
