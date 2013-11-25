#ifndef API_HELPERS_H_
#define API_HELPERS_H_

#include <EXTERN.h>
#include <perl.h>
#include <vector>

void define_constants(pTHX);

enum HintedCodeType
{
  hinted_sub,
  hinted_eval,
  hinted_main
};

class HintedCode
{
public:
  HintedCode() {}

  std::vector<OP *> hinted_cops(pTHX_ const char *hint_name) const;

  CV *cv;
  OP *root;
  OP *start;
  HintedCodeType type;
};

#endif
