#include	<stdlib.h>
#include	"rdfl.h"

int
test_1(void) {
  t_rdfl	example;
  ssize_t	s;
  void		*ptr;

  rdfl_init(&example);
  // Set options if needed
  readall_handler_t	rdfl_read;
  rdfl_set_timeout(&example, -1);
  rdfl_set_buffsize(&example, 200);
  if (!(rdfl_read = rdfl_load_path(&example, "/etc/passwd", RDFL_ALL_AVAILABLE, NULL)))
    return (EXIT_FAILURE);
  if ((s = rdfl_read(&example)) < 0) {
    rdfl_clean(&example);
    return (EXIT_FAILURE);
  }
  ptr = rdfl_flush_buffers_alloc(&example, &s);
  rdfl_clean(&example);

  write(1, ptr, s);
  free(ptr);
  return (EXIT_SUCCESS);
}
