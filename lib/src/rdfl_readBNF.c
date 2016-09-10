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

int
read_production(t_rdfl *obj) {
  char		*extract = NULL;

  fprintf(stderr, "readString: %zd\n", rdfl_ct_readString(obj, ((void **)&extract), RDFL_P_NULLTERMINATED | RDFL_P_CONSUME));
  if (extract) {
    fprintf(stdout, "=========== FINAL STRING: %s\n", extract);
    free(extract);
    return (1);
  }
  return (0);
}

int
rdfl_readBNF(t_rdfl *obj) {
  RDFL_OPT_SET(obj->settings, RDFL_AUTOCLEAR_BLANKS);
  if (rdfl_set_comment(obj, ";", "\n") != ERR_NONE
      || rdfl_set_comment(obj, "/*", "*/") != ERR_NONE)
    return (EXIT_FAILURE);
  while (read_production(obj)) {
    /*
    fprintf(stderr,"\n-----------------\n");
    rdfl_printbufferstate(obj);
    */
  }
  fprintf(stderr,"\nEND\n");
  rdfl_printbufferstate(obj);
  return (EXIT_SUCCESS);
}
