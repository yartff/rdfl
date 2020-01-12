#include	<stdlib.h>
#include	<unistd.h>
#include	"clean.h"

void
_close_fd(t_rdfl *obj) {
  if (RDFL_OPT_ISSET(obj->settings, RDFL_AUTOCLOSE)
      || RDFL_OPT_ISSET(obj->settings, LOCAL_OPEN))
    close(obj->fd);
  RDFL_OPT_UNSET(obj->settings, LOCAL_OPEN);
  RDFL_OPT_SET(obj->settings, LOCAL_EOF);
}

void
_comments_clean(t_comments *obj) {
  if (!obj)
    return ;
  _comments_clean(obj->next);
  free(obj->beg);
  free(obj->end);
  free(obj);
}
