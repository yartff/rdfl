#include	<stdlib.h>
#include	"rdfl.h"
#include	"rdfl_consumer.h"
#include	"rdfl_buffer_access.h"
#include	"rdfl_readBNF.h"

int
test_8__BNFparsing(void) {
  t_rdfl		example;
  t_rdfl_bnf		*bnf;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 14);
  if (rdfl_load_path(&example, "example_files/test_8_bash.bnf", RDFL_AUTOREAD, NULL))
    return (EXIT_FAILURE);
  // rdfl_printbufferstate(&example);
  bnf = rdfl_readBNF(&example);
  //fprintf(stderr, "=== Final RES: %d\n", rdfl_readBNF(&example));
  rdfl_printbufferstate(&example);
  rdfl_readBNF_dump(bnf);
  rdfl_freeBNF(bnf);
  rdfl_clean(&example);
  return (0);
}
