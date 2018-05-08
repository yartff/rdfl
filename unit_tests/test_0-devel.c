#include	<stdlib.h>
#include	"rdfl_devel.h"

int
test_0__devel(void) {
  rdflDevel_printbufferstate(NULL); // should be compiled in DEVEL
  // mode if declared in rdfl_devel.h
  return (EXIT_SUCCESS);
}
