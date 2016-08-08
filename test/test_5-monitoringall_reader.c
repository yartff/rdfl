#include	<stdlib.h>
#include	"rdfl.h"

int
test_5__monitoringall_reader(void) {
  t_rdfl			example;
  readmonitoringall_handler_t	rdfl_read;
  e_rdflerrors			err;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 500);
  rdfl_set_timeout(&example, 5000000);
  rdfl_read = get_func(RDFL_MONITORING | RDFL_ALL_AVAILABLE);
  fprintf(stderr, "%s\n", handler_typedef_declare(rdfl_read));
  if (rdfl_load_connect(&example, "127.0.0.1", 8081, RDFL_MONITORING | RDFL_ALL_AVAILABLE, &err)) {
    printf("%d\n", err);
    return (EXIT_FAILURE);
  }
  ssize_t	i = 0;
  while ((i = rdfl_read(&example, NULL)) > 0) {
    printf("RET: %zi\n", i);
    rdfl_printbufferstate(&example);
  }
  printf("RET: %zi\n", i);
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
