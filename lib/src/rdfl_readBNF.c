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

void
read_production(t_rdfl *obj...) {
  read_rule_name(); // '<' BT_identifier '>'
  read_operator("::=");
  read_rule_content();
}

void
rdfl_readBNF(t_rdfl *obj) {
  t_rdfl_csm		consumer;

  rdfl_consumer_init(&consumer, RDFL_P_CONSUME | RDFL_P_AUTOCLEAR_BLANKS);
  rdfl_consumer_add_commentNL(&consumer, ";");

  while (read_production(&obj, &consumer));

  return (0);
}
#endif

int
rdfl_readBNF(t_rdfl *obj) {
  void *extract;
  ssize_t i;

  //fprintf(stderr, "%d\n", rdfl_bacc_readptr(obj, "\"1234567890\"\nabcd", 10));
  if (rdfl_ct_readString(obj, &extract, RDFL_P_CONSUME | RDFL_P_NULLTERMINATED) > 0) {
    fprintf(stderr, "Extract: [%p]\n", (char *)extract);
    free(extract);
  }
  return (1);
}
