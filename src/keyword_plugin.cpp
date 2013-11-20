#include "keyword_plugin.h"

#include <EXTERN.h>
#include <perl.h>
#include "ppport.h"

/* For chaining the actual keyword plugin */
int (*GLOBAL_next_keyword_plugin)(pTHX_ char *, STRLEN, OP **);

static int my_keyword_plugin(pTHX_ char *keyword_ptr, STRLEN keyword_len, OP **op_ptr);


void
init_keyword_plugin(pTHX)
{
  GLOBAL_next_keyword_plugin = PL_keyword_plugin;
  PL_keyword_plugin = my_keyword_plugin;
}

static int
my_keyword_plugin(pTHX_ char *keyword_ptr, STRLEN keyword_len, OP **op_ptr)
{
  int ret;

  if (keyword_len == 3 && memcmp(keyword_ptr, "use", 3) == 0) {
    /*SAVETMPS;
    parse(aTHX_ op_ptr);
    ret = KEYWORD_PLUGIN_STMT;
    FREETMPS;
    */
    ret = (*GLOBAL_next_keyword_plugin)(aTHX_ keyword_ptr, keyword_len, op_ptr);
  } else {
    ret = (*GLOBAL_next_keyword_plugin)(aTHX_ keyword_ptr, keyword_len, op_ptr);
  }

  return ret;
}
