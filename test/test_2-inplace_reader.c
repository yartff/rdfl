#include	<stdlib.h>
#include	"rdfl.h"

int
test_2(void) {
  t_rdfl			example;
  void				*ptr;
  size_t			s, total, consume = 0;
  readnoextend_handler_t	rdfl_read;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 100);
  rdfl_read = rdfl_load_path(&example, "/etc/passwd", RDFL_NO_EXTEND);

  for (;;) {
    if (rdfl_read(&example, consume) == -1)
      return (EXIT_FAILURE);
    ptr = rdfl_getinplace_next_chunk(&example, &s, &total);
    if (!s) return (EXIT_SUCCESS);
    consume = (s < 12) ? (s) : (s / 2);
    write(1, ptr, consume);
  }
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
