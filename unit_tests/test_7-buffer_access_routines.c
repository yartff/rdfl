#include	<stdlib.h>
#include	"tester.h"
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
  printf("\n{result: %d}\n", rdfl_bacc_cmp_needdata(&example, STR, sizeof(STR) - 1, RDFL_P_CONSUME));
  rdflDevel_printbufferstate(&example);
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
