#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"unit_lib.h"
#include	"unit_opt.h"

/*
 * 0: do not change
 * -1: opt error
 * other: recompile in this mode
*/

e_buildMode
_get_buildMode_opt(void) {
  const char	**cli = opt_arg('m', NULL);
  e_buildMode	optmode;

  if (!cli) return 0;
  if (!strncmp(*cli, OPT_M_SET_DEB, (sizeof(OPT_M_SET_DEB) - 1))) {
    optmode |= MODE_DEBUG;
  } else if (!strncmp(*cli, OPT_M_SET_DEV, (sizeof(OPT_M_SET_DEV) - 1))) {
    optmode |= MODE_DEVEL;
  } else if (!strncmp(*cli, OPT_M_SET_BOTH, (sizeof(OPT_M_SET_BOTH) - 1))) {
    optmode |= MODE_DEBUG | MODE_DEVEL;
  } else {
    return (-1);
  }
  if (!strcmp((*cli) + strlen(*cli) - (sizeof(OPT_M_ADD_OPTI) - 1), OPT_M_ADD_OPTI))
    optmode |= MODE_OPTI;
  return (optmode);
}

e_unit_return_code
_check_buildMode(e_buildMode optmode, LOGP) {
  e_buildMode libmode = l__rdflBuild_getMode();
  if (libmode == optmode) return (SUCCESS);
  else {
    LOG_T("wrong shared lib mode");
    return (FAILURE);
  }
}

e_unit_return_code
_build_rdfl(e_buildMode mode, LOGP) {
  int dev = 0, deb = 0, opti = 0;
  if (mode & MODE_DEVEL) dev = 1;
  if (mode & MODE_DEBUG) deb = 1;
  if (mode & MODE_OPTI) opti = 1;

  char *make_mode = ((dev && deb) ? "both" :
      (dev) ? "devel" :
      (deb) ? "debug" : "re");
  char *make_opti = opti ? "O=1" : "";
  char cmd[32];

  sprintf(cmd, "make %s %s", make_mode, make_opti);
  if (system(cmd) != 0) {
    LOG_T("make command failure");
    return (FAILURE);
  }
  return (SUCCESS);
}

