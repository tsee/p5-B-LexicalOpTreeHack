#include "candidate_cvs.h"

#include <EXTERN.h>
#include <perl.h>
#include "ppport.h"

#include <set>
#include <map>
#include <string>
#include <utility>

using namespace std;

struct HintInfo
{
  SV *callback;
  set<CV *> candidate_cvs;

  HintInfo(SV *_callback) : callback(_callback) { }
};

typedef map<string, HintInfo> HintMap;

static HintMap LO_handlers; // needs to be per-interpreter
static const string LO_prefix = "lexical_optree_hack/";

void
register_hint(const char *hint, SV *callback)
{
  string full_name = LO_prefix + hint;

  if (!LO_handlers.insert(make_pair(full_name, HintInfo(callback))).second)
    croak("Hint '%s' already registered", hint);
  SvREFCNT_inc(callback);
}


void
enable_hint(const char *hint, bool enable)
{
  string full_name = LO_prefix + hint;
  SV **value = hv_fetch(GvHV(PL_hintgv), full_name.c_str(), full_name.size(), 1);

  if (value)
    sv_setiv_mg(*value, enable);
}


void
add_candidate_cv(pTHX_ CV *cv)
{
  // TODO for now ignore all special blocks, since most of them are
  // going to be freed early; we might still want to apply the rewrite
  // to END blocks
  if (CvSPECIAL(cv))
    return;

  for (HintMap::iterator handler = LO_handlers.begin(), end = LO_handlers.end();
       handler != end; ++handler)
    handler->second.candidate_cvs.insert(cv);
}


void
add_candidate_cv_if_hint_enabled(pTHX_ CV *cv)
{
  if (CvSPECIAL(cv))
    return;

  for (HintMap::iterator handler = LO_handlers.begin(), end = LO_handlers.end();
       handler != end; ++handler) {
    SV *hint = cophh_fetch_pvn(PL_compiling.cop_hints_hash,
                               handler->first.c_str(), handler->first.size(), 0, 0);

    if (hint != &PL_sv_placeholder && SvTRUE(hint))
      handler->second.candidate_cvs.insert(cv);
  }
}


static std::string
get_cv_name(pTHX_ CV *cv)
{
  if (CvGV(cv))
    return string(GvNAME(CvGV(cv)));
#if PERL_VERSION >= 18
  else if (CvNAME_HEK(*it))
    return string(CvNAME_HEK(cv)->hek_key);
#endif
  else
    return string("eval/main");
}


void
process_candidate_cvs(pTHX)
{
  typedef set<CV *>::const_iterator iterator;

  for (HintMap::iterator handler = LO_handlers.begin(), end = LO_handlers.end();
       handler != end; ++handler) {
    set<CV *> &candidate_cvs = handler->second.candidate_cvs;

    dSP;

    ENTER;
    SAVETMPS;

    EXTEND(SP, candidate_cvs.size());
    PUSHMARK(SP);

    for (iterator it = candidate_cvs.begin(), end = candidate_cvs.end();
         it != end; ++it)
      PUSHs(sv_2mortal(newRV_inc((SV *) *it)));

    PUTBACK;

    call_sv(handler->second.callback, G_VOID);

    FREETMPS;
    LEAVE;

    candidate_cvs.clear();
  }
}
