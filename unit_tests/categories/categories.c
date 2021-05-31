#include	<stdio.h>
#include	"unit_opt.h"

/*
const char *categories[] = {
}
// need devel
*/

static
void
_exec(const char *category) {
  fprintf(stdout, "[EXE] category.%s\n", category);
}

void
execute_categories(void) {
  unsigned int len;
  const char **c = opt_arg('c', &len);
  if (!c) return ;
  for (unsigned int i = 0; i < len; ++i) {
    _exec(c[i]);
  }
}
