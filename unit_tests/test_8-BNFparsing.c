#include	<stdlib.h>
#include	"tester.h"
#include	"rdfl_consumer.h"
#include	"rdfl_buffer_access.h"
#include	"rdfl_bnf.h"
#include	"rdfl_devel.h"

#include	"rdfl_context.h"

static
int
routine(const char *bnf_path, const char *bnf_applied) {
  t_rdfl		example;
  t_rdfl_bnf		*bnf = NULL;
  (void)bnf_applied;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 38);
  if (rdfl_load_path(&example, bnf_path, RDFL_AUTOREAD, NULL))
    return (EXIT_FAILURE);
  bnf = rdfl_readBNF(&example);
  rdfl_readBNF_dump(bnf);
  rdfl_freeBNF(bnf);
  rdflDevel_printbufferstate(&example);
  example.data.consumer.skip = 0;
  rdfl_clean(&example);
  return (0);
}

int
test_8__BNFparsing(void) {
  // t_rdfl		feed;

  /*
  rdfl_init(&feed);
  rdfl_set_buffsize(&feed, 15);
  if (rdfl_load_path(&feed, "example_files/test_8_apply.txt",
	RDFL_CONTEXT | RDFL_AUTOREAD | RDFL_AUTOCLEAR_BLANKS, NULL))
    return (EXIT_FAILURE);
    */
  // fprintf(stdout, "\n[[%zd]]\n", rdfl_applyBNF(bnf, &feed, "redirection"));
  // rdflDevel_printbufferstate(&feed);
  // rdfl_clean(&feed);
  //routine("example_files/test_8_bash.bnf", NULL);
  routine("example_files/test_8_ebnf.bnf", NULL);
  return (0);
}
