#include	<stdlib.h>
#include	"rdfl.h"
#include	"rdfl_buffer_access.h"

int
test_6__buffer_access(void) {
  t_rdfl			example;
  size_t			i = 0;
  readsize_handler_t		rdfl_read;
  int				tab[] = {
     23, -20, 30 // first buff rollover
    //50, -20 // buff segmented
  };

  rdfl_init(&example);
  rdfl_set_buffsize(&example, 2);
  if (rdfl_load_path(&example, "/etc/passwd", RDFL_FORCEREADSIZE, NULL))
    return (EXIT_FAILURE);
  rdfl_read = get_func(RDFL_FORCEREADSIZE);
  while (i < (sizeof(tab) / sizeof(*tab))) {
    if (tab[i] < 0)
      rdfl_force_consume_size(&example, -(tab[i]));
    else
      rdfl_read(&example, tab[i]);
    ++i;
  }
  rdfl_printbufferstate(&example);
  printf("\n{result: %d}\n", rdfl_bacc_cmp(&example, "t:/bin/tcsh\nbin:x:1:1:bin:/bin", 15));
  char	*str = rdfl_bacc_getallcontent(&example, &i);
  write(1, "[", 1); write(1, str, i); write(1, "]\n", 2);
  free(str);
  printf("%c\n", rdfl_bacc_ndx(&example, 3));
  rdfl_clean(&example);
  return (EXIT_SUCCESS);
}
