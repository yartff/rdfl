#include	<stdlib.h>
#include	"rdfl.h"
#include	"rdfl_consumer.h"
#include	"rdfl_buffer_access.h"

int
test_8__BNFparsing(void) {
  t_rdfl		example;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 128);
  if (rdfl_load_path(&example, "example_files/test_8_stringsfile.txt", RDFL_AUTOREAD, NULL))
    return (EXIT_FAILURE);
  // rdfl_printbufferstate(&example);
  rdfl_readBNF(&example);
  //fprintf(stderr, "=== Final RES: %d\n", rdfl_readBNF(&example));
  //rdfl_printbufferstate(&example);
  rdfl_clean(&example);
  return (0);
}
