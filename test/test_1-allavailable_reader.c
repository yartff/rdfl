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
  rdfl_read = rdfl_load_path(&example, "/etc/passwd", RDFL_ALL_AVAILABLE);
  if ((s = rdfl_read(&example)) == -1) {
    rdfl_clean(&example);
    return (EXIT_FAILURE);
  }

  ptr = rdfl_flush_buffers_alloc(&example, &s);
  rdfl_clean(&example);
  write(1, "TEST1:\n", sizeof("TEST1:\n") - 1);
  write(1, ptr, s);
  write(1, "END TEST1:\n", sizeof("END TEST1:\n") - 1);
  free(ptr);
  return (EXIT_SUCCESS);
}
