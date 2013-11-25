#include "api_helpers.h"

#include <string>
#include <COPFinder.h>

using namespace std;

static const string LO_prefix = "lexical_optree_hack/";

#define INT_CONST(name) \
  define_int_constant(aTHX_ name, #name)

static void
define_int_constant(pTHX_ IV value, const char *name)
{
  char buffer[64];

  strcpy(buffer, "B::LexicalOpTreeHack::");
  strcat(buffer, name);

  SV *sv = get_sv(buffer, GV_ADD);
  HV *hv = gv_stashpvs("B::LexicalOpTreeHack", GV_ADD);

  sv_setiv(sv, value);
  newCONSTSUB(hv, name, sv);

  AV *export_ok = get_av("B::LexicalOpTreeHack::EXPORT_OK", GV_ADD);
  av_push(export_ok, newSVpv(name, 0));
}

void
define_constants(pTHX)
{
  INT_CONST(hinted_sub);
  INT_CONST(hinted_eval);
  INT_CONST(hinted_main);
}

std::vector<OP *>
HintedCode::hinted_cops(pTHX_ const char *hint_name) const
{
    COPFinder copf(hint_name);
    copf.visit(aTHX_ root, root);
    return copf.get_cops();
}

