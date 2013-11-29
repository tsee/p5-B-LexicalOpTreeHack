#include "helemfast.h"

/* From B::Generate */
#ifndef PadARRAY
# if PERL_VERSION < 8 || (PERL_VERSION == 8 && !PERL_SUBVERSION)
typedef AV PADLIST;
typedef AV PAD;
# endif
# define PadlistARRAY(pl)	((PAD **)AvARRAY(pl))
# define PadlistNAMES(pl)	(*PadlistARRAY(pl))
# define PadARRAY		AvARRAY
#endif
 
XOP HF_helemfast_op;
XOP HF_helemfast_lex_op;
XOP HF_helemfast_lex_pad_op;

typedef struct {
  BASEOP
  // Either one of the following may be used to identify the key
  SV *op_keysv;
  PADOFFSET op_keyoffset;
  U32 op_keyhash;
} helemfast_op_t;

static OP *
hf_pp_helemfast(pTHX)
{
  dVAR; dSP;
  // FIXME This cGVOP_gv business requires a particular setup from upstream, NOT working yet
  HV * const hv = GvHVn(cGVOP_gv);
  // FIXME lval
  const U32 lval = PL_op->op_flags & OPf_MOD;
  // FIXME there's likely a recent perl's hv_common_key_len or so that could be more efficient
  HE * const ent = hv_fetch_ent(hv, ((helemfast_op_t *)PL_op)->op_keysv, lval, ((helemfast_op_t *)PL_op)->op_keyhash);
  SV *sv = HeVAL(ent);
  EXTEND(SP, 1);
  if (!lval && SvRMAGICAL(hv) && SvGMAGICAL(sv)) /* see note in pp_helem() */
      mg_get(sv);
  PUSHs(sv);
  RETURN;
}

static OP *
hf_pp_helemfast_lex_pad(pTHX)
{
  dVAR; dSP;
  HV * const hv = MUTABLE_HV(PAD_SV(PL_op->op_targ));
  // FIXME lval was patched out of this for performance testing
  // FIXME there's likely a recent perl's hv_common_key_len or so that could be more efficient
  //sv_dump(((helemfast_op_t *)PL_op)->op_keysv);
  HE * const ent = hv_fetch_ent(hv, PAD_SVl(((helemfast_op_t *)PL_op)->op_keyoffset), 0, ((helemfast_op_t *)PL_op)->op_keyhash);
  SV *sv = HeVAL(ent);
  EXTEND(SP, 1);
  if (SvRMAGICAL(hv) && SvGMAGICAL(sv)) /* see note in pp_helem() */
      mg_get(sv);
  PUSHs(sv);
  RETURN;
}

static OP *
hf_pp_helemfast_lex(pTHX)
{
  dVAR; dSP;
  HV * const hv = MUTABLE_HV(PAD_SV(PL_op->op_targ));
  // FIXME lval
  const U32 lval = PL_op->op_flags & OPf_MOD;
  // FIXME there's likely a recent perl's hv_common_key_len or so that could be more efficient
  //sv_dump(((helemfast_op_t *)PL_op)->op_keysv);
  HE * const ent = hv_fetch_ent(hv, ((helemfast_op_t *)PL_op)->op_keysv, lval, ((helemfast_op_t *)PL_op)->op_keyhash);
  SV *sv = HeVAL(ent);
  EXTEND(SP, 1);
  if (!lval && SvRMAGICAL(hv) && SvGMAGICAL(sv)) /* see note in pp_helem() */
      mg_get(sv);
  PUSHs(sv);
  RETURN;
}

static OP *
hf_alloc_helemfast(pTHX_ const bool is_lexical, const bool use_padoffset)
{
  helemfast_op_t *op;

  NewOp(1101, op, 1, helemfast_op_t);
  op->op_type = (OPCODE)OP_CUSTOM;
  op->op_next = (OP *)op;
  op->op_private = 0;
  op->op_flags = 0;
  op->op_targ = 0;

  op->op_keysv = NULL;
  op->op_keyhash = 0;

  if (is_lexical) {
    if (use_padoffset)
      op->op_ppaddr = hf_pp_helemfast_lex_pad;
    else
      op->op_ppaddr = hf_pp_helemfast_lex;
  }
  else
    op->op_ppaddr = hf_pp_helemfast;

  return (OP *)op;
}

OP *
hf_prepare_helemfast_lex(pTHX_ CV *cv, PADOFFSET hash_padoffset, SV *key)
{
  helemfast_op_t *op = (helemfast_op_t *)hf_alloc_helemfast(aTHX_ true, false);

  // FIXME Does this need a SvREFCNT_inc(key) ?
  op->op_targ = hash_padoffset;
  op->op_keysv = key;
  op->op_keyoffset = 0;

  STRLEN len;
  char *str = SvPVbyte(key, len);
  PERL_HASH(op->op_keyhash, str, len);

  return (OP *)op;
}

OP *
hf_prepare_helemfast_lex_padkey(pTHX_ CV *cv, PADOFFSET hash_padoffset, PADOFFSET key_padoffset)
{
  helemfast_op_t *op = (helemfast_op_t *)hf_alloc_helemfast(aTHX_ true, true);

  // FIXME Does this need a SvREFCNT_inc(key) ?
  op->op_targ = hash_padoffset;
  op->op_keysv = NULL;
  op->op_keyoffset = key_padoffset;
  op->op_keyhash = 0; // FIXME precompute hash key

  // Fake up curpad to be able to access PAD SV for hash precomputation
  SV *keysv;
  {
    ENTER;
    SAVECOMPPAD(); // restores both PL_comppad and PL_curpad

    PL_comppad = PadlistARRAY(CvPADLIST(cv))[1];
    PL_curpad = AvARRAY(PL_comppad);

    keysv = PAD_SVl(key_padoffset);

    LEAVE;
  }

  STRLEN len;
  char *str = SvPVbyte(keysv, len);
  PERL_HASH(op->op_keyhash, str, len);

  return (OP *)op;
}

// FIXME implement hf_prepare_helemfast

void
hf_init_global_state(pTHX)
{
  /* Setup our custom op */
  XopENTRY_set(&HF_helemfast_op, xop_name, "helemfast");
  XopENTRY_set(&HF_helemfast_op, xop_desc, "helemfast");
  XopENTRY_set(&HF_helemfast_op, xop_class, OA_BASEOP); /* ??? */

  XopENTRY_set(&HF_helemfast_lex_op, xop_name, "helemfast_lex");
  XopENTRY_set(&HF_helemfast_lex_op, xop_desc, "helemfast for lexical hashes");
  XopENTRY_set(&HF_helemfast_lex_op, xop_class, OA_BASEOP); /* ??? */

  XopENTRY_set(&HF_helemfast_lex_pad_op, xop_name, "helemfast_lex_pad");
  XopENTRY_set(&HF_helemfast_lex_pad_op, xop_desc, "helemfast for lexical hashes using a pad offset");
  XopENTRY_set(&HF_helemfast_lex_pad_op, xop_class, OA_BASEOP); /* ??? */

  Perl_custom_op_register(aTHX_ hf_pp_helemfast, &HF_helemfast_op);
  Perl_custom_op_register(aTHX_ hf_pp_helemfast_lex, &HF_helemfast_lex_op);
  Perl_custom_op_register(aTHX_ hf_pp_helemfast_lex_pad, &HF_helemfast_lex_pad_op);
}
