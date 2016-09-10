#include		<stdlib.h>
#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"

/*
[ test ] -> test is optionnal
{ test } -> 0 or more tests
| -> OR

If several rules have the same name, consider this as OR
*/

#if 0
void
read_expr() {
  read_optionnal_designator(); // [ expr ]
  // OR
  read_optionnal_designator(); // [ expr ]
}

void
read_rule_content() {
  read_expr();
}

#endif

#define		OP_RULE			"::="
#define		OP_OR			"|"
#define		READ_OPE(obj, c, opt)	(rdfl_bacc_readptr(obj, c, sizeof(c) - 1, opt))
int
read_production(t_rdfl *obj) {
  e_bacc_options	opt = RDFL_P_NULLTERMINATED | RDFL_P_CONSUME;
  char			*extract = NULL;
  int			ret;

  if (rdfl_ct_readIdentifier(obj, ((void **)&extract), opt) <= 0
    || !extract)
    return (EXIT_FAILURE);
  if ((ret = READ_OPE(obj, OP_RULE, opt)) <= 0
     /* || (ret = read_or_expression(obj)) <= 0*/) {
    free(extract);
    return (EXIT_FAILURE);
  }
  free(extract);
  return (EXIT_SUCCESS);
}

int
rdfl_readBNF(t_rdfl *obj) {
  RDFL_OPT_SET(obj->settings, RDFL_AUTOCLEAR_BLANKS);
  if (rdfl_set_comment(obj, ";", "\n") != ERR_NONE
      || rdfl_set_comment(obj, "/*", "*/") != ERR_NONE)
    return (EXIT_FAILURE);
  while (read_production(obj) == EXIT_SUCCESS) {
    /*
    fprintf(stderr,"\n-----------------\n");
    rdfl_printbufferstate(obj);
    */
  }
  rdfl_printbufferstate(obj);
  return (EXIT_SUCCESS);
}
