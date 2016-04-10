#include	<stdlib.h>
#include	"rdfl.h"

int
test_4(void) {
  t_rdfl	example;
  readmonitoring_handler_t	rdfl_read;
  e_rdflerrors	err;

  rdfl_init(&example);
  rdfl_set_timeout(&example, 5000000);
  if (!(rdfl_read = rdfl_load_connect(&example, "127.0.0.1", 3232,
	  RDFL_MONITORING | RDFL_ALL_AVAILABLE, NULL)))
    return (EXIT_FAILURE);
  printf("Size: %zu\n", rdfl_read(&example, &err));
  printf("ERR: %i\n", err);
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
