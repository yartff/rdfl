#include	<stdlib.h>
#include	"rdfl.h"

int
main() {
  t_rdfl	example;

  rdfl_init(&example);
  // Set options if needed
  printf("%s", handler_typedef_declare(rdfl_load(&example, 0, RDFL_ALL_AVAILABLE))); printf("\n");
  return (EXIT_SUCCESS);
}
