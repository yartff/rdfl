#include	<stdlib.h>
#include	"rdfl.h"
#include	"rdfl_buffer_access.h"

#define STR "root:x:0:0:root:/root:/bin/tcsh"
int
test_7__buffer_access_routines(void) {
  t_rdfl			example;
  //size_t			i = 0;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 2);
  if (rdfl_load_path(&example, "/etc/passwd", RDFL_AUTOREAD, NULL))
    return (EXIT_FAILURE);
  rdfl_printbufferstate(&example);
  printf("\n{result: %d}\n", rdfl_bacc_readptr(&example, STR, sizeof(STR) - 1));
  rdfl_printbufferstate(&example);
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
