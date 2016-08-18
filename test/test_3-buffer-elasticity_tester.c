#include	<stdlib.h>
#include	"rdfl.h"

int
test_3__bufferelasticity_tester(void) {
  t_rdfl			example;
  size_t			i = 0;
  readsize_handler_t		rdfl_read;
  int				tab[] = {
    15, -10, -5, 4
  };

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 5);
  if (rdfl_load_path(&example, "../notes.txt", RDFL_FORCEREADSIZE | RDFL_FULLEMPTY, NULL))
    return (EXIT_FAILURE);
  rdfl_printbufferstate(&example);
  rdfl_read = get_func(RDFL_FORCEREADSIZE | RDFL_FULLEMPTY);
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
