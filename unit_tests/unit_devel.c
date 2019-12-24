#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"unit.h"
#include	"lib.h"
#include	"opt.h"

/*
 * 0: do not change
 * -1: opt error
 * other: recompile in this mode
*/

int
check_buildMode() {
  rdflBuild_getMode_t f_getMode = load_rdfl_function("rdflBuild_getMode", 1);
  e_buildMode libmode = f_getMode(), optmode = 0;
  const char	**opt = opt_arg('m', NULL);
  if (!opt) return 0;
  if (!strncmp(*opt, OPT_M_DEB, (sizeof(OPT_M_DEB) - 1))) {
    optmode |= MODE_DEBUG;
  } else if (!strncmp(*opt, OPT_M_DEV, (sizeof(OPT_M_DEV) - 1))) {
    optmode |= MODE_DEVEL;
  } else if (!strncmp(*opt, OPT_M_BOTH, (sizeof(OPT_M_BOTH) - 1))) {
    optmode |= MODE_DEBUG | MODE_DEVEL;
  } else {
    return (-1);
  }

  if (!strcmp((*opt) + strlen(*opt) - (sizeof(OPT_M_ADD_OPTI) - 1), OPT_M_ADD_OPTI))
    optmode |= MODE_OPTI;
  if (libmode == optmode)
    return (0);
  return (optmode);
}

int
recompile_rdfl(int mode) {
  clean_rdfl();
  int dev = 0, deb = 0, opti = 0;
  if (mode & MODE_DEVEL) dev = 1;
  if (mode & MODE_DEBUG) deb = 1;
  if (mode & MODE_OPTI) opti = 1;

  char *make_mode = ((dev && deb) ? "both" :
      (dev) ? "devel" :
      (deb) ? "debug" : "error");
  char *make_opti = opti ? "O=1" : "";
  char cmd[32];

  sprintf(cmd, "make %s %s", make_mode, make_opti);
  if (system(cmd) != 0) {
    fprintf(stderr, "make command could not be executed\n");
    return (EXIT_FAILURE);
  }
  if (load_rdfl() == EXIT_FAILURE)
    return (EXIT_FAILURE);
  if (check_buildMode()) {
    fprintf(stderr, "Wrong mode after recompile\n");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
