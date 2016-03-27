#include	<stdlib.h>
#include	<stdio.h>
#include	"rdfl.h"

int
main() {
  t_rdfl	example;
  ssize_t	s;
  void		*ptr;

  rdfl_init(&example);
  // Set options if needed
  readall_handler_t	rdfl_read;
  rdfl_set_timeout(&example, -1);
  rdfl_set_buffsize(&example, 3000);
  rdfl_read = rdfl_load_path(&example, "/etc/passwd", RDFL_ALL_AVAILABLE);
  if ((s = rdfl_read(&example)) == -1) {
    rdfl_clean(&example);
    return (EXIT_FAILURE);
  }

  ptr = rdfl_flush_buffers(&example, NULL);
  rdfl_clean(&example);
  write(1, ptr, s);
  free(ptr);
  return (EXIT_SUCCESS);
}
