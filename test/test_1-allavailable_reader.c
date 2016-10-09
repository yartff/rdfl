#include	<stdlib.h>
#include	"tester.h"

int
test_1__allavailable_reader(void) {
  t_rdfl		example;
  ssize_t		s;
  void			*ptr;
  e_rdflerrors		err = 0;
  readall_handler_t	rdfl_read;

  rdfl_init(&example);
  // Set options if needed
  rdfl_set_timeout(&example, -1);
  rdfl_set_buffsize(&example, 200);
  if ((rdfl_load_path(&example, "/etc/passwd", RDFL_ALL_AVAILABLE, &err))) {
    printf("%d\n", err);
    return (EXIT_FAILURE);
  }
  rdfl_read = get_func(RDFL_ALL_AVAILABLE);
  if ((s = rdfl_read(&example, NULL)) < 0) {
    rdfl_clean(&example);
    return (EXIT_FAILURE);
  }
  ptr = rdfl_flush_buffers_alloc(&example, &s);
  rdfl_clean(&example);
  write(1, ptr, s);
  free(ptr);
  return (EXIT_SUCCESS);
}
