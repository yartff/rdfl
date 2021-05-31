#include	<stdio.h>
#include	"unit_opt.h"

static
void
_exec(const char *public) {
  fprintf(stdout, "[EXE]   public.%s\n", public);
}

void
execute_public(void) {
  unsigned int len;
  const char **p = opt_arg('p', &len);
  if (!p) return ;
  for (unsigned int i = 0; i < len; ++i) {
    _exec(p[i]);
  }
}
