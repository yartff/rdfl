#include	<stdlib.h>
#include	"unit_mock.h"
#include	"unit_lib.h"

/*
void output_definition(t_rdfl_buffer *r, const char *varname);

void
mock_output_definition(t_rdfl *rdfl) {
  output_definition(&rdfl->data, "OUTPUT_RDFL");
}
*/

t_rdfl *
get_mock() {
  t_rdfl *rdfl = l__rdfl_init_new();
  l__rdfl_set_buffsize(rdfl, 35);
  l__rdfl_load_path(rdfl, "unit_tests/example_files/text/file", RDFL_ALL_AVAILABLE, NULL);
  l__rdflDevel_printbufferstate(rdfl);
  // ssize_t total = l__rdflReader_allavail(rdfl, NULL);
  // fprintf(stdout, "all clear {%zi}\n", total);
  // mock_output_definition(rdfl);
  return (rdfl);
}

/*
t_rdfl		*
mock_get_rdfl(e_unit_mocktype type) {
  t_rdfl	*rdfl = rdfl_init_new();
  rdfl
}

t_rdfl_buffer	*
mock_get_buffer(e_unit_mocktype type) {
  t_rdfl_buffer	*
}

void
mock_clean_rdfl(t_rdfl *b) {
}

void
mock_clean_buffer(t_rdfl_buffer *b) {
}
*/
