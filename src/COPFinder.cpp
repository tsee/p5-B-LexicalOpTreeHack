#include "COPFinder.h"
#include "ppport.h"
#include "candidate_cvs.h"
#include <string>

COPFinder::COPFinder()
  : hint_name(NULL)
{}

COPFinder::COPFinder(const char *_hint_name)
{
  hint_name = _hint_name ? new std::string(_hint_name) : NULL;
}

COPFinder::~COPFinder()
{
  if (hint_name)
    delete hint_name;
}

OPTreeVisitor::visit_control_t
COPFinder::visit_op(pTHX_ OP *o, OP *parentop)
{
  if (o
      && OP_CLASS(o) == OA_COP
      && (hint_name == NULL || cop_has_hint(aTHX_ (COP *)o, hint_name->c_str())) )
  {
    cops.push_back(o);
  }
  return OPTreeVisitor::VISIT_CONT;
}

const std::vector<OP *> &
COPFinder::get_cops() const
{
  return cops;
}
