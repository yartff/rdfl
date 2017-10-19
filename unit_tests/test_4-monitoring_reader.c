#include	<stdlib.h>
#include	"tester.h"

int
test_4__monitoring_reader(void) {
  t_rdfl	example;
  readmonitoring_handler_t	rdfl_read;
  e_rdflerrors	err;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 500);
  rdfl_read = get_func(RDFL_MONITORING);
  fprintf(stderr, "%s\n", handler_typedef_declare(rdfl_read));
  if (rdfl_load_connect(&example, "127.0.0.1", 8080, RDFL_MONITORING, &err)) {
    printf("%d\n", err);
    return (EXIT_FAILURE);
  }
  ssize_t	i = 0;
  while ((i = rdfl_read(&example)) > 0) {
    printf("RET: %zi\n", i);
    rdfl_printbufferstate(&example);
  }
  printf("RET: %zi\n", i);
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
