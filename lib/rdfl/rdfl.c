#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>
#include	"rdfl.h"
#include	"rdfl_devel.h"
#include	"buffer_read.h"
// #include	"context.h"

// API infos
//
inline
size_t
rdfl_get_total_datasize(t_rdfl *obj) {
  return (obj->data.consumer.total);
}

// API Utils
//

inline int	rdfl_eofreached(t_rdfl *obj)
{ return (RDFL_OPT_ISSET(obj->settings, LOCAL_REACHED_EOF)); }

inline
void *
rdfl_getinplace_next_chunk(t_rdfl *obj, size_t *s, size_t *total_s) {
  void		*ptr;

  ptr = b_consumer_ptr(&obj->data, s);
  if (total_s) *total_s = obj->data.consumer.total;
  return (ptr);
}

#if 0
ssize_t
rdfl_read_ignore_size(t_rdfl *obj, size_t s) {
  (void)obj, (void)s; // if monitoring etc...
  return (-1);
}

inline
void
rdfl_force_consume_size(t_rdfl *obj, size_t s) {
  rdfl_b_consume_size(&obj->data, s);
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FULLEMPTY))
    b_fullclean_if_empty(&obj->data);
}
#endif

// API Readers
//
ssize_t
rdflReader_singlestep(t_rdfl *obj) {
  void		*ptr;
  size_t	available;

  if (!(ptr = b_buffer_getchunk_extend(&obj->data, &available,
	  (size_t)obj->v.buffsize)))
    return (ERR_MEMORY_ALLOC);
  return (push_read(obj, ptr, available));
}

ssize_t
rdflReader_size(t_rdfl *obj, size_t count) {
  void		*ptr;
  ssize_t	s;
  size_t	available, total = count;

  while (count) {
    if (!(ptr = b_buffer_getchunk_extend(&obj->data, &available,
	    (RDFL_OPT_ISSET(obj->settings, RDFL_ADJUST_BUFFSIZE)
	     ? count : (size_t)obj->v.buffsize))))
      return (ERR_MEMORY_ALLOC);
    if (available > count) available = count;
    if ((s = push_read(obj, ptr, available)) < 0)
      return (s);
    if (!s || s != ((ssize_t)available))
      return (total - (count - s));
    count -= (size_t)s;
  }
  return (total);
}

ssize_t
rdflReader_all_available(t_rdfl *obj, e_rdflerrors *err) {
  size_t	available;
  ssize_t	full = 1, total = 0;
  void		*ptr;

  while (full) {
    if (!(ptr = b_buffer_getchunk_extend(&obj->data, &available, obj->v.buffsize))) {
      if (err) {
	*err = ERR_MEMORY_ALLOC;
	return (total);
      }
      return (ERR_MEMORY_ALLOC);
    }
    if ((full = push_read(obj, ptr, available)) <= 0) {
      if (err) {
	*err = (!full ? VAL_LAST_READ_0 : full);
	return (total);
      }
      if (!full || full < VAL || full == ERR_CONNECTION_CLOSED)
	return (total);
      return (full);
    }
    total += full;
    full = ((size_t)full == available);
  }
  return (total);
}

ssize_t
rdflReader_noextend(t_rdfl *obj, size_t consume) {
  size_t	s;
  ssize_t	total;
  void		*ptr;

  rdfl_b_consume_size(&obj->data, consume);
  ptr = b_buffer_getchunk(&obj->data, &s);
  if (!s) { return (ERR_NOSPACELEFT); }
  if ((total = push_read(obj, ptr, s)) < 0)
    return (total);

// TODO
#if 0
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FILLFREESPACE) && total == s) {
    ptr = b_buffer_getchunk(&obj->data, &s);
    if (!s) return ((size_t)ret);
    if ((ret = push_read(&obj, ptr, s)) < 0) {
      if (err) *err = ret;
      return (total);
    }
  }
#endif

  return (total);
}

ssize_t
rdflReader_legacy(t_rdfl *obj, void *buf, size_t count) {
  (void)obj, (void)buf, (void)count;
  return (0);
}

ssize_t
rdflReader_monitoring_no_extend(t_rdfl *obj) {
  return (nw_monitoring(obj->nw, &read_into_chunk, obj));
}

ssize_t
rdflReader_monitoring_allavail(t_rdfl *obj, e_rdflerrors *err) {
  ssize_t	total = 0;
  nwret_t	nw_ret;

  while ((nw_ret = nw_monitoring(obj->nw, &read_into_chunk_extend, obj)) > 0)
    total += nw_ret;
  if (nw_ret <= 0) {
    if (err) {
      *err = (nw_ret ? nw_ret : VAL_LAST_READ_0);
      return (total);
    }
    return (nw_ret > VAL ? nw_ret : total);
  }
  return (total);
}

ssize_t
rdflReader_monitoring(t_rdfl *obj) {
  return (nw_monitoring(obj->nw, &read_into_chunk_extend, obj));
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

// Init
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

// Opt setters
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
