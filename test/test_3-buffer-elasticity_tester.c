#include	<stdlib.h>
#include	"rdfl.h"

int
test_3__bufferelasticity_tester(void) {
  t_rdfl			example;
  size_t			i = 0;
  readsize_handler_t		rdfl_read;
  int				tab[] = {
    50, -10, 30, -10, 200, -45,
  };

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 64);
  if (!(rdfl_read = rdfl_load_path(&example, "/etc/passwd", RDFL_FORCEREADSIZE, NULL)))
    return (EXIT_FAILURE);
  while (i < (sizeof(tab) / sizeof(*tab))) {
    if (tab[i] < 0)
      rdfl_force_consume_size(&example, -(tab[i]));
    else if (tab[i] > 0)
      rdfl_read(&example, tab[i]);
    rdfl_printbufferstate(&example);
    ++i;
  }

  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
