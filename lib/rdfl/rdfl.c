#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>
#include	"rdfl.h"
#include	"values.h"
#include	"buffer.h"
#include	"clean.h"
#include	"network.h"

// Init
//

void	rdfl_init(t_rdfl *dest) {
#ifdef	DEVEL
  if (!dest) return ; // TODO: return Dev Err
#endif
  static t_rdfl_values		v = {
    .timeout = RDFL_DEFAULT_TIMEOUT,
    .buffsize = RDFL_DEFAULT_BUFFSIZE,
  };
  memcpy(&(dest->v), &v, sizeof(dest->v));
  dest->nw = NULL;
  dest->settings = RDFL_NONE;
  dest->fd = -1;
#ifdef	DEVEL
  fprintf(stdout, "RDFL Devel version. Read the wiki (later...) for more infos.\n");
#endif
}

t_rdfl	*rdfl_init_new(void) {
  t_rdfl	*new = malloc(sizeof(*new));
  if (!new) return (NULL);
  rdfl_init(new);
  RDFL_OPT_SET(new->settings, LOCAL_ALLOC);
  return (new);
}

// Options
//

int	rdfl_set_timeout(t_rdfl *r, ssize_t timeout) {
  r->v.timeout = timeout < -1 ? RDFL_DEFAULT_TIMEOUT : timeout;
  return ((r->nw) ? nw_init_timeout(r->nw, timeout) : ERR_NONE);
}
void	rdfl_set_buffsize(t_rdfl *r, ssize_t buffsize)
{ r->v.buffsize = ((buffsize == 0) ? RDFL_DEFAULT_BUFFSIZE : buffsize); }

int
rdfl_add_comment(t_rdfl *r, const char *begin, const char *end) {
  t_comments		*new;
  char			*strb = NULL, *stre;

  if (!(new = malloc(sizeof(*new)))
      || !(strb = strdup(begin))
      || !(stre = strdup(end))) {
    free(new);
    free(strb);
    return (ERR_MEMORY_ALLOC);
  }
  new->next = r->v.cmts;
  r->v.cmts = new;
  new->beg = strb;
  new->end = stre;
  return (ERR_NONE);
}

// Loaders
//

int
rdfl_load(t_rdfl *new, int fd, e_rdflsettings settings, e_rdflerrors *err) {
  e_rdflerrors	nw_ret;

  new->fd = fd;
  new->settings = settings;
  if (RDFL_OPT_ISSET(settings, RDFL_MONITORING)) {
    if (!(new->nw = malloc(sizeof(*(new->nw))))) {
      if (err) *err = ERR_MEMORY_ALLOC;
      return (EXIT_FAILURE);
    }
    new->nw->timeout = NULL;
    if ((nw_ret = nw_init(new->nw, fd, new->v.timeout)) != ERR_NONE) {
      if (err) *err = nw_ret;
      return (EXIT_FAILURE);
    }
  }
  if (b_init(&(new->data),
	((RDFL_OPT_ISSET(settings, RDFL_ADJUST_BUFFSIZE)
	  || RDFL_OPT_ISSET(settings, RDFL_FULLEMPTY)) ?
	 0 : new->v.buffsize)) == EXIT_FAILURE) {
    if (err) *err = ERR_MEMORY_ALLOC;
    return (EXIT_FAILURE);
  }
  /* TODO: ctx
  new->data.consumer.ctx = NULL;
  if (RDFL_OPT_ISSET(settings, RDFL_CONTEXT) && rdfl_context_init(&(new->data)) < 0) {
    return (EXIT_FAILURE);
  }
  */
  if (err) *err = ERR_NONE;
  return (EXIT_SUCCESS);
  // return (_check_func(settings));
}

int
rdfl_load_fileptr(t_rdfl *new, FILE *stream, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd_type;

  if ((fd_type = fileno(stream)) == -1) {
    if (err) *err = ERR_BADF;
    return (EXIT_FAILURE);
  }
  return (rdfl_load(new, fd_type, settings, err));
}

int
rdfl_load_path(t_rdfl *new, const char *path, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd;

  if ((fd = open(path, O_RDONLY)) == -1) {
    if (err) *err = ERR_OPEN;
    return (EXIT_FAILURE);
  }
  RDFL_OPT_SET(settings, LOCAL_OPEN);
  return (rdfl_load(new, fd, settings, err));
}

int
rdfl_load_connect(t_rdfl *new, const char *ip, int port, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd_sock;

  if ((fd_sock = nw_openconnection(ip, port)) < 0) {
    if (err) *err = fd_sock;
    return (EXIT_FAILURE);
  }
  RDFL_OPT_SET(settings, LOCAL_OPEN);
  return (rdfl_load(new, fd_sock, settings, err));
}

// Clean
//

rdfl_retval
rdfl_clean(t_rdfl *obj) {
#ifdef	DEVEL
  if (!obj) return (ERRDEV_NULLOBJECT);
#endif
  if (obj->nw != NULL) {
    nw_clean(obj->nw);
    free(obj->nw);
  }
  _comments_clean(obj->v.cmts);
  _close_fd(obj);
  b_clean(&obj->data);
  if (RDFL_OPT_ISSET(obj->settings, LOCAL_ALLOC))
    free(obj);
#ifdef	DEVEL
  return (ERR_NONE);
#endif
}
