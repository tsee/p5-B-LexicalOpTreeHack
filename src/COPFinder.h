#ifndef FT_COPFINDER_H_
#define FT_COPFINDER_H_

#include "EXTERN.h"
#include "perl.h"
#include "OpTreeVisitor.h"
#include "OpTreeVisitorPerlCb.h"
#include <vector>
#include <string>

class HintedCOPList;

class COPFinder : public OpTreeVisitor {
public:
  // No hint name means "don't filter by hint, return all COPs"
  COPFinder();
  // Hint name is automatically prefixed by the value of LO_prefix
  COPFinder(const char *_hint_name);
  ~COPFinder();

  virtual visit_control_t visit_op(pTHX_ OP *o, OP *parentop);
  const std::vector<OP *> &get_cops() const;

private:
  HintedCOPList *cop_list;
};

#endif
