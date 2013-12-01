#ifndef FT_OPTREEVISITORPERLCB_H_
#define FT_OPTREEVISITORPERLCB_H_

#include "EXTERN.h"
#include "perl.h"

#include "OpTreeVisitor.h"

class OpTreeVisitorPerlCb : public OpTreeVisitor {
public:
  OpTreeVisitorPerlCb(pTHX_ CV *callback);
  ~OpTreeVisitorPerlCb();

  visit_control_t visit_op(pTHX_ OP *o, OP *parentop);

private:
  // Need to keep threading context around for destructor which doesn't take a pTHX
#ifdef PERL_IMPLICIT_CONTEXT
  tTHX perl_thread_context;
#endif
  CV *perl_callback; 
};

#endif
