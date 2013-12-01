#include "COPFinder.h"
#include "ppport.h"
#include "candidate_cvs.h"
#include <string>

using namespace std;


// Data structure for maintaining a list of all COPs with a particular hint
class HintedCOPList {
public:
  // Constructor variant that only tracks COPs with the given hint.
  HintedCOPList(const char *_hint_name);
  // Constructor variant that tracks all COPs.
  HintedCOPList();
  ~HintedCOPList();

  // Possibly adds OP to list of hinted COPs. Returns whether it was added or not.
  bool maybe_add_op(pTHX_ OP *o);

  const std::vector<OP *> &get_cops() const;
  void clear_cops();

private:
  std::string *hint_name;
  std::vector<OP *> cops;
  COPHH *prev_hints_hash;
  bool prev_hh_had_hint;
};

HintedCOPList::HintedCOPList()
  : hint_name(NULL), prev_hints_hash(NULL), prev_hh_had_hint(false)
{}

HintedCOPList::HintedCOPList(const char *_hint_name)
  : prev_hints_hash(NULL), prev_hh_had_hint(false)
{
  hint_name = _hint_name
              ? new string(LO_prefix + string(_hint_name))
              : NULL;
}

HintedCOPList::~HintedCOPList()
{
  if (hint_name)
    delete hint_name;
}

const std::vector<OP *> &
HintedCOPList::get_cops() const
{
  return cops;
}

void
HintedCOPList::clear_cops()
{
  cops.clear();
  prev_hints_hash = NULL;
  prev_hh_had_hint = false;
}

bool
HintedCOPList::maybe_add_op(pTHX_ OP *o)
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
}





COPFinder::COPFinder()
{}

COPFinder::COPFinder(const char *_hint_name)
  : cop_list(new HintedCOPList(_hint_name))
{}

COPFinder::~COPFinder()
{
  delete cop_list;
}

OpTreeVisitor::visit_control_t
COPFinder::visit_op(pTHX_ OP *o, OP *parentop)
{
  (void)parentop;
  cop_list->maybe_add_op(aTHX_ o);
  return OpTreeVisitor::VISIT_CONT;
}

const std::vector<OP *> &
COPFinder::get_cops() const
{
  return cop_list->get_cops();
}

