#include	<stdio.h>
#include	<stdlib.h>
#include	"unit.h"
#include	"unit_devel.h"
#include	"lib.h"
#include	"opt.h"
#include	"public.h"

void
execute_tests(const char **files) {
  (void)files;
#if 0
  unsigned int i = 0;
  while (files[i]) {
    printf("%s\n", files[i]);
    ++i;
  }
#endif
  t_rdfl *rdfl = l__rdfl_init_new();
  l__rdfl_set_buffsize(rdfl, 35);
  l__rdfl_load_path(rdfl, "unit_tests/example_files/text/file", RDFL_ALL_AVAILABLE, NULL);
  fprintf(stdout, "load_path OK\n");
  ssize_t total = l__rdflReader_allavail(rdfl, NULL);
  fprintf(stdout, "all clear {%zi}\n", total);
  l__rdfl_consume_size(rdfl, total - 1);
  l__rdflDevel_printbufferstate(rdfl);
}

int
main(int argc, char **argv) {
  if (init_opt(argc, argv) == EXIT_FAILURE) {
    return (EXIT_FAILURE);
  }
  if (load_rdfl() == EXIT_FAILURE) {
    return (EXIT_FAILURE);
  }
  int build_ret = check_buildMode();
  if (build_ret == -1) {
    opt_helpopt('m');
    return (EXIT_FAILURE);
  }
  print_opt();
  if (build_ret && recompile_rdfl(build_ret) == EXIT_FAILURE)
    return (EXIT_FAILURE);

  load_public_functions();
  execute_tests(opt_args());

  clean_opt();
  clean_rdfl();
  return (EXIT_SUCCESS);
}
