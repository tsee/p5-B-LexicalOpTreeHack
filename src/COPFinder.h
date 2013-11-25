#ifndef FT_COPFINDER_H_
#define FT_COPFINDER_H_

#include "EXTERN.h"
#include "perl.h"
#include "OPTreeVisitor.h"
#include <vector>
#include <string>

/* Code relating to traversing and manipulating the OP tree */

class COPFinder : public OPTreeVisitor {
public:
  // No hint name means "don't filter by hint, return all COPs"
  COPFinder();
  COPFinder(const char *_hint_name);
  ~COPFinder();
  virtual visit_control_t visit_op(pTHX_ OP *o, OP *parentop);
  const std::vector<OP *> &get_cops() const;

private:
  std::vector<OP *> cops;
  std::string *hint_name;
};

#endif
