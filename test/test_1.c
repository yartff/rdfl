#include	<stdlib.h>
#include	<stdio.h>
#include	"rdfl.h"

int
main() {
  t_rdfl	example;

  rdfl_init(&example);
  // Set options if needed
  readall_handler_t	rdfl_read;
  rdfl_read = handler_typedef_declare(rdfl_load(&example,
	  0, RDFL_ALL_AVAILABLE));
  return (EXIT_SUCCESS);
}
