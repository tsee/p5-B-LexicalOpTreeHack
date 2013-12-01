#include "OpTreeVisitorPerlCb.h"
#include "ppport.h"

#include <assert.h>

#include "opclass.h"

#ifdef PERL_IMPLICIT_CONTEXT
#   define OTV_dTHX tTHX my_perl = this->perl_thread_context
#else
#   define OTV_dTHX
#endif

OpTreeVisitorPerlCb::OpTreeVisitorPerlCb(pTHX_ CV *callback)
{
#ifdef PERL_IMPLICIT_CONTEXT
  perl_thread_context = aTHX;
#endif

  if (callback == NULL)
    croak("Need callback function to invoke for each OP");

  SvREFCNT_inc(callback);
  perl_callback = callback;
}

OpTreeVisitorPerlCb::~OpTreeVisitorPerlCb()
{
  OTV_dTHX;
  SvREFCNT_dec(perl_callback);
}

// Separated from invoke_perl_callback for inheritance
OpTreeVisitor::visit_control_t
OpTreeVisitorPerlCb::visit_op(pTHX_ OP *o, OP *parentop)
{
  return invoke_perl_callback(aTHX_ o, parentop);
}

OpTreeVisitor::visit_control_t
OpTreeVisitorPerlCb::invoke_perl_callback(pTHX_ OP *o, OP *parentop)
{
  dSP;
  const unsigned int nargs = 2;
  SV *sv;
  OpTreeVisitor::visit_control_t retval;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);

  unsigned int i;
  EXTEND(SP, nargs);

  // Push the current OP onto Perl stack
  sv = sv_newmortal();
  sv_setiv(newSVrv(sv, cc_opclassname(aTHX_ o)), PTR2IV(o));
  PUSHs(sv);

  // Push the current OP's parent onto Perl stack
  sv = sv_newmortal();
  sv_setiv(newSVrv(sv, cc_opclassname(aTHX_ parentop)), PTR2IV(parentop));
  PUSHs(sv);

  // Invoke
  PUTBACK;
  const int count = call_sv((SV *)perl_callback, G_SCALAR);
  SPAGAIN;

  // Get status flag from return value
  if (count == 0)
      croak("Invalid return value of OP tree walker callback: "
            "Need to return any of VISIT_CONT, VISIT_SKIP, VISIT_ABORT");
  else {
    SV *retv = POPs;
    if (retv == NULL || retv == &PL_sv_undef)
      croak("Invalid return value of OP tree walker callback: "
            "Need to return any of VISIT_CONT, VISIT_SKIP, VISIT_ABORT");
    retval = (OpTreeVisitor::visit_control_t)SvIV(retv);
    if (retval != VISIT_CONT && retval != VISIT_SKIP && retval != VISIT_ABORT)
      croak("Invalid return value of OP tree walker callback: "
            "Need to return any of VISIT_CONT, VISIT_SKIP, VISIT_ABORT");
  }

  PUTBACK;
  FREETMPS;
  LEAVE;

  return retval;
}

