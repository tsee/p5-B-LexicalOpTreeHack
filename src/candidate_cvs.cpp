#include "candidate_cvs.h"

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
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

#define MY_CXT_KEY "lexical_optree_hack"
typedef struct {
  HintMap *hint_handlers;
} my_cxt_t;

START_MY_CXT

static const string LO_prefix = "lexical_optree_hack/";


static void
teardown_candidate_structures(pTHX_ void *ptr)
{
  (void)ptr;
  dMY_CXT;
  delete MY_CXT.hint_handlers;
}


void
init_candidate_structures(pTHX)
{
  MY_CXT_INIT;
  MY_CXT.hint_handlers = new HintMap();
  Perl_call_atexit(aTHX_ teardown_candidate_structures, NULL);
}


void
register_hint(pTHX_ const char *hint, SV *callback)
{
  string full_name = LO_prefix + hint;

  dMY_CXT;
  HintMap &handlers = *MY_CXT.hint_handlers;

  if (!handlers.insert(make_pair(full_name, HintInfo(callback))).second)
    croak("Hint '%s' already registered", hint);
  SvREFCNT_inc(callback);
}


void
enable_hint(pTHX_ const char *hint, bool enable)
{
  string full_name = LO_prefix + hint;
  SV **value = hv_fetch(GvHV(PL_hintgv), full_name.c_str(), full_name.size(), 1);

  if (value)
    sv_setiv_mg(*value, enable);

  dMY_CXT;
  HintMap &handlers = *MY_CXT.hint_handlers;
  CV *compcv = PL_compcv;
  // in older Perls, PL_compiling points to the BEGIN block being executed,
  // and we need to reach to the compiling subroutine through CvOUTSIDE.
  // Perls newer than 5.17.5 (commit 85ffec368212c6) fixed that.
#if PERL_VERSION < 18
  compiling = CvOUTSIDE(compcv);
#endif
  if (enable) {
    HintMap::iterator entry = handlers.find(full_name);

    if (entry != handlers.end())
      entry->second.candidate_cvs.insert(compcv);
  }
}


void
add_candidate_cv_if_hint_enabled(pTHX_ CV *cv)
{
  if (CvSPECIAL(cv))
    return;

  dMY_CXT;
  HintMap &handlers = *MY_CXT.hint_handlers;

  for (HintMap::iterator handler = handlers.begin(), end = handlers.end();
       handler != end; ++handler) {
    SV *hint = cophh_fetch_pvn(PL_compiling.cop_hints_hash,
                               handler->first.c_str(), handler->first.size(), 0, 0);

    if (hint != &PL_sv_placeholder && SvTRUE(hint))
      handler->second.candidate_cvs.insert(cv);
  }
}


void
process_candidate_cvs(pTHX)
{
  typedef set<CV *>::const_iterator iterator;

  dMY_CXT;
  HintMap &handlers = *MY_CXT.hint_handlers;

  for (HintMap::iterator handler = handlers.begin(), end = handlers.end();
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
