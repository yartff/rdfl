#include	<stdlib.h>
#include	"rdfl.h"

int
test_3__bufferelasticity_tester(void) {
  t_rdfl			example;
  size_t			i = 0;
  readsize_handler_t		rdfl_read;
  int				tab[] = {
    50, -10, 30, -10, 200, -45, 51, 1, -10, -59, -190, 50, 42, -1, 1, -4, 5, 0
  };

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 100);
  if (rdfl_load_path(&example, "/dev/urandom", RDFL_FORCEREADSIZE, NULL))
    return (EXIT_FAILURE);
  rdfl_read = get_func(RDFL_FORCEREADSIZE);
  while (i < (sizeof(tab) / sizeof(*tab))) {
    if (tab[i] < 0)
      rdfl_force_consume_size(&example, -(tab[i]));
    else
      rdfl_read(&example, tab[i]);
    rdfl_printbufferstate(&example);
    ++i;
  }
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
