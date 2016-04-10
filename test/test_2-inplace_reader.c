#include	<stdlib.h>
#include	"rdfl.h"

int
test_2__inplace_reader(void) {
  t_rdfl			example;
  void				*ptr;
  size_t			s, total, consume = 0;
  readnoextend_handler_t	rdfl_read;
  ssize_t			ret;

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 94);
  if (!(rdfl_read = rdfl_load_path(&example, "/etc/passwd", RDFL_NO_EXTEND, NULL)))
    return (EXIT_FAILURE);
  // srand(time(NULL));

  for (;;) {
    if ((ret = rdfl_read(&example, consume)) < 0 && ret != ERR_NOSPACELEFT) {
      rdfl_clean(&example);
      return (EXIT_FAILURE);
    }
    ptr = rdfl_getinplace_next_chunk(&example, &s, &total);
    if (!s) break ;
    consume = s > 30 ? (rand() % (s + 1)) : s;
    write(1, ptr, consume);
  }
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
