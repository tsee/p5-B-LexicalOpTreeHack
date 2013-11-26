#include "COPFinder.h"
#include "ppport.h"
#include "candidate_cvs.h"
#include <string>

using namespace std;

COPFinder::COPFinder()
  : hint_name(NULL), prev_hints_hash(NULL), prev_hh_had_hint(false)
{}

COPFinder::COPFinder(const char *_hint_name)
  : prev_hints_hash(NULL), prev_hh_had_hint(false)
{
  hint_name = _hint_name
              ? new string(LO_prefix + string(_hint_name))
              : NULL;
}

COPFinder::~COPFinder()
{
  if (hint_name)
    delete hint_name;
}

OPTreeVisitor::visit_control_t
COPFinder::visit_op(pTHX_ OP *o, OP *parentop)
{
  // Not a pretty if/else if chain, with similar bodies,
  // but keeps things from becoming a ridiculous condition.
  if (o && OP_CLASS(o) == OA_COP)
  {
    COPHH *hh;

    if (hint_name == NULL) {
      cops.push_back(o);
    }
    else if (( hh = ((COP *)o)->cop_hints_hash) ) {
      // Optimization: Hit on our hobo-cache
      if (hh == prev_hints_hash && prev_hh_had_hint) {
        cops.push_back(o);
        prev_hints_hash = hh;
        prev_hh_had_hint = true;
      }
      else {
        // Could possibly optimize further by remembering the hash value for the hint name
        // Effectively inlined cop_has_hint()
        // Annoying: This creates and returns a mortal SV every time :(
        SV *hint_value = cophh_fetch_pvn(hh, hint_name->c_str(), hint_name->length(), 0, 0);
        if (hint_value && hint_value != &PL_sv_placeholder && SvTRUE(hint_value))
        {
          cops.push_back(o);
          prev_hints_hash = hh;
          prev_hh_had_hint = true;
        }
        else {
          prev_hints_hash = hh;
          prev_hh_had_hint = false;
        }
      }
    }
  }

  return OPTreeVisitor::VISIT_CONT;
}

const std::vector<OP *> &
COPFinder::get_cops() const
{
  return cops;
}
