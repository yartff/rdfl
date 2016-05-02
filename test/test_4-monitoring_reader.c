#include	<stdlib.h>
#include	"rdfl.h"

int
test_4__monitoring_reader(void) {
  t_rdfl	example;
  readmonitoring_handler_t	rdfl_read;
  e_rdflerrors	err;

  rdfl_init(&example);
  rdfl_set_timeout(&example, 5000000);
  rdfl_set_buffsize(&example, 50000);
  if (!(rdfl_read = rdfl_load_path(&example, "big_file.txt",
	  RDFL_MONITORING | RDFL_ALL_AVAILABLE, NULL)))
    return (EXIT_FAILURE);
  printf("Size: %zu\n", rdfl_read(&example, &err));
  rdfl_printbufferstate(&example);
  printf("ERR: %i\n", err);
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
