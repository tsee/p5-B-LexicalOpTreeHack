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
  const std::string *get_hint_name() const;
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
  hint_name = _hint_name == NULL
              ? NULL
              : new string(LO_prefix + string(_hint_name));
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

const std::string *
HintedCOPList::get_hint_name() const
{
  return hint_name;
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
  bool retval = false;
  // Not a pretty if/else if chain, with similar bodies,
  // but keeps things from becoming a ridiculous condition.
  // FIXME this can be refactored a lot nicer with early returns
  if (!o || OP_CLASS(o) != OA_COP)
    return false;

  if (hint_name == NULL) {
    cops.push_back(o);
    return true;
  }

  COPHH *hh = ((COP *)o)->cop_hints_hash;

  if (hh == NULL)
    return false;

  // Optimization: Hit on our hobo-cache
  if (hh == prev_hints_hash && prev_hh_had_hint) {
    cops.push_back(o);
    prev_hints_hash = hh;
    prev_hh_had_hint = true;
    return true;
  }

  // Could possibly optimize further by remembering the hash value for the hint name
  // Effectively inlined cop_has_hint()
  // Annoying: This creates and returns a mortal SV every time :(
  SV *hint_value = cophh_fetch_pvn(hh, hint_name->c_str(), hint_name->length(), 0, 0);
  if (hint_value && hint_value != &PL_sv_placeholder && SvTRUE(hint_value))
  {
    cops.push_back(o);
    prev_hints_hash = hh;
    prev_hh_had_hint = true;
    return true;
  }
  else {
    prev_hints_hash = hh;
    prev_hh_had_hint = false;
    return true;
  }

  return false;
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



COPFinderPerlCb::COPFinderPerlCb(pTHX_ CV *callback)
  : OpTreeVisitorPerlCb(aTHX_ callback),
    cop_list(new HintedCOPList())
{}

COPFinderPerlCb::COPFinderPerlCb(pTHX_ CV *callback, const char *_hint_name)
  : OpTreeVisitorPerlCb(aTHX_ callback),
    cop_list(new HintedCOPList(_hint_name))
{}

COPFinderPerlCb::~COPFinderPerlCb()
{
  delete cop_list;
}

OpTreeVisitor::visit_control_t
COPFinderPerlCb::visit_op(pTHX_ OP *o, OP *parentop)
{
  if (cop_list->maybe_add_op(aTHX_ o))
    return invoke_perl_callback(aTHX_ o, parentop);

  return OpTreeVisitor::VISIT_CONT;
}

const std::vector<OP *> &
COPFinderPerlCb::get_cops() const
{
  return cop_list->get_cops();
}
