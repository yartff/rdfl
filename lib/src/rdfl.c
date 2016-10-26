#include	<fcntl.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	"rdfl_local.h"
#include	"rdfl_context.h"
#include	"rdfl_network.h"
#include	"rdfl.h"

static void _rdfl_close(t_rdfl *);
//
// Read wrappers utils
static
ssize_t
rdfl_push_read(t_rdfl *obj, void *ptr, size_t available) {
  ssize_t	ret;

  if (RDFL_OPT_ISSET(obj->settings, RDFL_LOC_REACHED_EOF))
    return (0);
  ret = rdfl_b_push_read(&obj->data, obj->fd, ptr, available);
  if (ret <= 0)
    _rdfl_close(obj);
  return (ret);
}

static
ssize_t
rdfl_read_into_chunk_extend(void *obj) {
  void		*ptr;
  size_t	s;

  if (!(ptr = rdfl_b_buffer_getchunk_extend(&((t_rdfl *)obj)->data, &s,
	  ((t_rdfl *)obj)->v.buffsize)))
    return (ERR_MEMORY);
  return (rdfl_push_read(obj, ptr, s));
}

#if 0
static
int
rdfl_lookup_freespace(t_rdfl *obj) {
  size_t	s;
  rdfl_b_buffer_getchunk(&obj->data, &s);
  return ((s != 0));
}
#endif

static
ssize_t
rdfl_read_into_chunk(void *obj) {
  void		*ptr;
  size_t	s;

  ptr = rdfl_b_buffer_getchunk(&((t_rdfl *)obj)->data, &s);
  if (!s)
    return (ERR_NOSPACELEFT);
  return (rdfl_push_read(obj, ptr, s));
}

// API infos
//
inline
size_t
rdfl_get_total_datasize(t_rdfl *obj) {
  return (obj->data.consumer.total);
}

// API Utils
//
ssize_t
rdfl_read_ignore_size(t_rdfl *obj, size_t s) {
  (void)obj, (void)s; // if monitoring etc...
  return (-1);
}

// API Consummers
//
void *
rdfl_flush_buffers_alloc(t_rdfl *obj, ssize_t *count_value) {
  void		*ptr;

  if (!(ptr = rdfl_b_consume_all_alloc(&obj->data, count_value))) {
    return (NULL);
  }
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FULLEMPTY))
    rdfl_b_fullclean_if_empty(&obj->data);
  return (ptr);
}

void *
rdfl_flush_firstbuffer_alloc(t_rdfl *obj, ssize_t *count_value) {
  void		*ptr;

  if (!(ptr = rdfl_b_consume_firstbuffer_alloc(&obj->data, count_value)))
    return (NULL);
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FULLEMPTY))
    rdfl_b_fullclean_if_empty(&obj->data);
  return (ptr);
}

void *
rdfl_getinplace_next_chunk(t_rdfl *obj, size_t *s, size_t *total_s) {
  void		*ptr;

  ptr = rdfl_b_next_chunk(&obj->data, s);
  *total_s = obj->data.consumer.total;
  return (ptr);
}

inline
void
rdfl_force_consume_size(t_rdfl *obj, size_t s) {
  rdfl_b_consume_size(&obj->data, s);
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FULLEMPTY))
    rdfl_b_fullclean_if_empty(&obj->data);
}

inline int	rdfl_eofreached(t_rdfl *obj)
{ return (RDFL_OPT_ISSET(obj->settings, RDFL_LOC_REACHED_EOF)); }

// API Readers
//
ssize_t
_read_singlestep(t_rdfl *obj) {
  void		*ptr;
  size_t	available;

  if (!(ptr = rdfl_b_buffer_getchunk_extend(&obj->data, &available,
	  (size_t)obj->v.buffsize)))
    return (ERR_MEMORY);
  return (rdfl_push_read(obj, ptr, available));
}

ssize_t
_read_size(t_rdfl *obj, size_t count) {
  void		*ptr;
  ssize_t	s;
  size_t	available, total = count;

  while (count) {
    if (!(ptr = rdfl_b_buffer_getchunk_extend(&obj->data, &available,
	    (RDFL_OPT_ISSET(obj->settings, RDFL_ADJUST_BUFFSIZE)
	     ? count : (size_t)obj->v.buffsize))))
      return (ERR_MEMORY);
    if (available > count) available = count;
    if ((s = rdfl_push_read(obj, ptr, available)) < 0)
      return (s);
    if (!s || s != ((ssize_t)available))
      return (total - (count - s));
    count -= (size_t)s;
  }
  return (total);
}

ssize_t
_read_all_available(t_rdfl *obj, e_rdflerrors *err) {
  size_t	available;
  ssize_t	full = 1, total = 0;
  void		*ptr;

  while (full) {
    if (!(ptr = rdfl_b_buffer_getchunk_extend(&obj->data, &available, obj->v.buffsize))) {
      if (err) {
	*err = ERR_MEMORY;
	return (total);
      }
      return (ERR_MEMORY);
    }
    if ((full = rdfl_push_read(obj, ptr, available)) <= 0) {
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
_read_noextend(t_rdfl *obj, size_t consume) {
  size_t	s;
  ssize_t	total = 0;
  void		*ptr;

  rdfl_b_consume_size(&obj->data, consume);
  ptr = rdfl_b_buffer_getchunk(&obj->data, &s);
  if (!s) { return (ERR_NOSPACELEFT); }
  if ((total = rdfl_push_read(obj, ptr, s)) < 0)
    return (total);
#if 0
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FILLFREESPACE) && total == s) {
    ptr = rdfl_b_buffer_getchunk(&obj->data, &s);
    if (!s) return ((size_t)ret);
    if ((ret = rdfl_push_read(&obj, ptr, s)) < 0) {
      if (err) *err = ret;
      return (total);
    }
  }
#endif
  return (total);
}

ssize_t
_read_legacy(t_rdfl *obj, void *buf, size_t count) {
  (void)obj, (void)buf, (void)count;
  return (0);
}

ssize_t
_read_monitoring_no_extend(t_rdfl *obj) {
  return (rdfl_nw_monitoring(obj->nw, &rdfl_read_into_chunk, obj));
}

ssize_t
_read_monitoring_allavail(t_rdfl *obj, e_rdflerrors *err) {
  ssize_t	total = 0;
  nwret_t	nw_ret;

  while ((nw_ret = rdfl_nw_monitoring(obj->nw, &rdfl_read_into_chunk_extend, obj)) > 0)
    total += nw_ret;
  if (nw_ret <= 0) {
    if (err) {
      *err = (nw_ret ? nw_ret : VAL_LAST_READ_0);
      // *err = ((nw_ret == 0) * VAL_LAST_READ_0 + nw_ret);
      return (total);
    }
    return (nw_ret > VAL ? nw_ret : total);
  }
  return (total);
}

ssize_t
_read_monitoring(t_rdfl *obj) {
  return (rdfl_nw_monitoring(obj->nw, &rdfl_read_into_chunk_extend, obj));
}

// Destructors
//
static
void
_rdfl_close(t_rdfl *obj) {
  if (RDFL_OPT_ISSET(obj->settings, RDFL_AUTOCLOSE)
      || RDFL_OPT_ISSET(obj->settings, RDFL_LOC_OPEN))
    close(obj->fd);
  RDFL_OPT_UNSET(obj->settings, RDFL_LOC_OPEN);
  RDFL_OPT_SET(obj->settings, RDFL_LOC_REACHED_EOF);
}

static
void
_rdfl_comments_clean(t_comments *obj) {
  if (!obj)
    return ;
  _rdfl_comments_clean(obj->next);
  free(obj->beg);
  free(obj->end);
  free(obj);
}

void
rdfl_clean(t_rdfl *obj) {
  if (!obj) return ;
  if (obj->nw != NULL) {
    rdfl_nw_clean(obj->nw);
    free(obj->nw);
  }
  _rdfl_comments_clean(obj->v.cmts);
  _rdfl_close(obj);
  rdfl_buffer_clean(&obj->data);
  if (RDFL_OPT_ISSET(obj->settings, RDFL_LOC_ALLOC))
    free(obj);
}

// Constructors
//
rdflret_t
rdfl_load(t_rdfl *new, int fd, e_rdflsettings settings, e_rdflerrors *err) {
  e_rdflerrors	nw_ret;

  new->fd = fd;
  new->settings = settings;
  if (RDFL_OPT_ISSET(settings, RDFL_MONITORING)) {
    if (!(new->nw = malloc(sizeof(*(new->nw))))) {
      if (err) *err = ERR_MEMORY;
      return (EXIT_FAILURE);
    }
    new->nw->timeout = NULL;
    if ((nw_ret = rdfl_nw_init(new->nw, fd, new->v.timeout)) != ERR_NONE) {
      if (err) *err = nw_ret;
      return (EXIT_FAILURE);
    }
  }
  if (rdfl_buffer_init(&(new->data),
	((RDFL_OPT_ISSET(settings, RDFL_ADJUST_BUFFSIZE)
	  || RDFL_OPT_ISSET(settings, RDFL_FULLEMPTY)) ?
	 0 : new->v.buffsize)) == EXIT_FAILURE) {
    if (err) *err = ERR_MEMORY;
    return (EXIT_FAILURE);
  }
  new->data.consumer.ctx = NULL;
  if (RDFL_OPT_ISSET(settings, RDFL_CONTEXT) && rdfl_context_init(&(new->data)) < 0) {
    return (EXIT_FAILURE);
  }
  if (err) *err = ERR_NONE;
  return (EXIT_SUCCESS);
  // return (_check_func(settings));
}

rdflret_t
rdfl_load_fileptr(t_rdfl *new, FILE *stream, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd_type;

  if ((fd_type = fileno(stream)) == -1) {
    if (err) *err = ERR_BADF;
    return (EXIT_FAILURE);
  }
  return (rdfl_load(new, fd_type, settings, err));
}

rdflret_t
rdfl_load_path(t_rdfl *new, const char *path, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd;

  if ((fd = open(path, O_RDONLY)) == -1) {
    if (err) *err = ERR_OPEN;
    return (EXIT_FAILURE);
  }
  RDFL_OPT_SET(settings, RDFL_LOC_OPEN);
  return (rdfl_load(new, fd, settings, err));
}

rdflret_t
rdfl_load_connect(t_rdfl *new, const char *ip, int port, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd_sock;

  if ((fd_sock = rdfl_nw_openconnection(ip, port)) < 0) {
    if (err) *err = fd_sock;
    return (EXIT_FAILURE);
  }
  RDFL_OPT_SET(settings, RDFL_LOC_OPEN);
  return (rdfl_load(new, fd_sock, settings, err));
}

// Opt setters
//
void	rdfl_init(t_rdfl *dest) {
#ifdef DEVEL
  if (!dest) return ;
#endif
  static t_rdfl_values		v = {
    .timeout = RDFL_DEFAULT_TIMEOUT,
    .buffsize = RDFL_DEFAULT_BUFFSIZE,
  };
  memcpy(&(dest->v), &v, sizeof(dest->v));
  dest->nw = NULL;
  dest->settings = RDFL_NONE;
  dest->fd = -1;
#ifdef DEVEL
  fprintf(stdout, "RDFL Devel version. Read the wiki (later...) for more infos.\n");
#endif
}

t_rdfl	*rdfl_init_new(void) {
  t_rdfl	*new = malloc(sizeof(*new));
  if (!new) return (NULL);
  rdfl_init(new);
  RDFL_OPT_SET(new->settings, RDFL_LOC_ALLOC);
  return (new);
}

int	rdfl_set_timeout(t_rdfl *r, ssize_t timeout) {
  r->v.timeout = timeout < -1 ? RDFL_DEFAULT_TIMEOUT : timeout;
  return ((r->nw) ? rdfl_nw_init_timeout(r->nw, timeout) : ERR_NONE);
}
void	rdfl_set_buffsize(t_rdfl *r, ssize_t buffsize)
{ r->v.buffsize = ((buffsize == 0) ? RDFL_DEFAULT_BUFFSIZE : buffsize); }

int	rdfl_set_skip(t_rdfl *r, size_t skip) {
#ifdef		DEVEL
  if (RDFL_OPT_ISSET(r->settings, RDFL_CONTEXT))
    return (ERR_BADFLAGS);
#endif
  return (rdfl_b_set_skip(&r->data, skip));
}

int
rdfl_set_comment(t_rdfl *r, const char *begin, const char *end) {
  t_comments		*new;
  char			*strb = NULL, *stre;

  if (!(new = malloc(sizeof(*new)))
      || !(strb = strdup(begin))
      || !(stre = strdup(end))) {
    free(new);
    free(strb);
    return (ERR_MEMORY);
  }
  new->next = r->v.cmts;
  r->v.cmts = new;
  new->beg = strb;
  new->end = stre;
  return (ERR_NONE);
}
