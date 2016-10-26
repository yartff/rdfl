#include	<stdlib.h>
#include	"tester.h"
#include	"rdfl_consumer.h"
#include	"rdfl_buffer_access.h"
#include	"rdfl_readBNF.h"
#include	"rdfl_devel.h"

#include	"rdfl_context.h"
int
test_8__BNFparsing(void) {
  t_rdfl		example;
  // t_rdfl		feed;
  // t_rdfl_bnf		*bnf = NULL;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 140);
  if (rdfl_load_path(&example, "example_files/test_8_bash.bnf", RDFL_AUTOREAD | RDFL_ALL_AVAILABLE | RDFL_CONTEXT, NULL))
    return (EXIT_FAILURE);
  /*
  rdfl_init(&feed);
  rdfl_set_buffsize(&feed, 15);
  if (rdfl_load_path(&feed, "example_files/test_8_apply.txt",
	RDFL_CONTEXT | RDFL_AUTOREAD | RDFL_AUTOCLEAR_BLANKS, NULL))
    return (EXIT_FAILURE);
    */
  printf("%zd\n", _read_all_available(&example, NULL));
  // bnf = rdfl_readBNF(&example);
  // rdfl_readBNF_dump(bnf);
  // fprintf(stdout, "\n[[%zd]]\n", rdfl_applyBNF(bnf, &feed, "redirection"));
  // rdfl_printbufferstate(&feed);
  // rdfl_freeBNF(bnf);



  rdfl_set_skip(&example, 30);
  rdfl_pushcontext(&example);
  rdfl_set_skip(&example, 38);
  rdfl_pushcontext(&example);
  rdfl_set_skip(&example, 30);
  rdfl_pushcontext(&example);


  rdfl_printbufferstate(&example);
  rdfl_clean(&example);
  // rdfl_clean(&feed);
  return (0);
}
