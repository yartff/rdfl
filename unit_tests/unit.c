#include	<stdio.h>
#include	<stdlib.h>
#include	"unit_opt.h"
#include	"unit_lib.h"
#include	"unit_public.h"
#include	"unit_file.h"
#include	"unit_categories.h"

void
execute(const char **cmd) {
#if 1
  if (!cmd[0]) return ;
  unsigned int i = 0;
  while (cmd[i]) {
    fprintf(stdout, "%s ", cmd[i]);
    ++i;
  }
  fprintf(stdout, "\n");
#endif
#if 0
  t_rdfl *rdfl = l__rdfl_init_new();
  l__rdfl_set_buffsize(rdfl, 35);
  l__rdfl_load_path(rdfl, "unit_tests/example_files/text/file", RDFL_ALL_AVAILABLE, NULL);
  fprintf(stdout, "load_path OK\n");
  ssize_t total = l__rdflReader_allavail(rdfl, NULL);
  fprintf(stdout, "all clear {%zi}\n", total);
  l__rdfl_consume_size(rdfl, total - 1);
  l__rdflDevel_printbufferstate(rdfl);
  mock_output_definition(rdfl);
#endif
}

int
main(int argc, char **argv) {
  if (init_opt(argc, argv)
      || init_lib())
    return (EXIT_FAILURE);

  // opt_print();
  execute(opt_args());

  execute_public();
  execute_categories();
  execute_files();

  clean_opt();
  clean_rdfl();
  return (EXIT_SUCCESS);
}
