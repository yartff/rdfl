#include	<stdlib.h>
#include	"rdfl.h"

int
test_3(void) {
  t_rdfl			example;
  void				*ptr;
  size_t			s, total, consume = 0;
  readnoextend_handler_t	rdfl_read;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 60);
  if (!(rdfl_read = rdfl_load_path(&example, "/etc/passwd", RDFL_NO_EXTEND, NULL)))
    return (EXIT_FAILURE);

  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
