#include	<fcntl.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	"rdfl_local.h"
#include	"rdfl_network.h"

//
// Read wrappers utils
static
size_t
rdfl_read_into_chunk_extend(t_rdfl *obj, e_rdflerrors *err) {
  void		*ptr;
  size_t	s;
  ssize_t	ret;

  if (!(ptr = rdfl_b_buffer_getchunk_extend(&obj->data, &s, obj->v.buffsize))) {
    if (err) *err = ERR_MEMORY;
    return (0);
  }
  if ((ret = rdfl_b_push_read(&obj->data, obj->fd, ptr, s)) < 0) {
    if (err != NULL) *err = ret;
    return (0);
  }
  if (err) {
    if (ret == 0) *err = ERR_CONNECTION_CLOSED;
    if ((size_t)ret == s) *err = VAL_POTENTIALDATA;
  }
  return (ret);
}

static
size_t
rdfl_read_into_chunk(t_rdfl *obj, e_rdflerrors *err) {
  void		*ptr;
  size_t	s;
  ssize_t	ret;

  ptr = rdfl_b_buffer_getchunk(&obj->data, &s);
  if (!s) {
    if (err) *err = ERR_NOSPACELEFT;
    return (0);
  }
  if ((ret = rdfl_b_push_read(&obj->data, obj->fd, ptr, s)) < 0) {
    if (err != NULL) *err = ret;
    return (0);
  }
  if (err) {
    if (ret == 0) *err = ERR_CONNECTION_CLOSED;
    if ((size_t)ret == s) *err = VAL_POTENTIALDATA;
  }
  return (ret);
}

static
size_t
rdfl_monitoring_read(t_rdfl *obj, t_rdfl_net *nw, e_rdflerrors *err) {
  size_t	(*fct)(t_rdfl *, e_rdflerrors *);
  int		nw_ret;
  size_t	total = 0;
  e_rdflerrors	localerr = ERR_NONE;

  fct = (RDFL_OPT_ISSET(obj->settings, RDFL_NO_EXTEND)
      ? &rdfl_read_into_chunk
      : &rdfl_read_into_chunk_extend);

  if ((nw_ret = rdfl_nw_monitoring(nw)) > 0) {
    total = fct(obj, &localerr);
    if (localerr == VAL_POTENTIALDATA) {
      if (RDFL_OPT_ISSET(obj->settings, RDFL_FILLFREESPACE)) {
	if ((nw_ret = rdfl_nw_monitoring(nw)) > 0) {
	  total += fct(obj, err);
	} else if (err) *err = nw_ret;
      }
    } else if (localerr != ERR_NONE && err) *err = localerr;
  } else if (err) *err = nw_ret;
  return (total);
}

static
size_t
rdfl_monitoring_allavail(t_rdfl *obj, t_rdfl_net *nw, e_rdflerrors *err) {
  size_t	total = 0, ret;
  int		nw_ret;

  while ((nw_ret = rdfl_nw_monitoring(nw)) > 0) {
    if ((ret = rdfl_read_into_chunk_extend(obj, err)) == 0) {
      if (err) *err = ERR_CONNECTION_CLOSED;
      return (total);
    }
    total += ret;
  }
  if (nw_ret != 0 && err) *err = nw_ret;
  return (total);
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

void
rdfl_force_consume_size(t_rdfl *obj, size_t s) {
  rdfl_b_consume_size(&obj->data, s);
}

// API Readers
//
static
int
_read_size(t_rdfl *obj, size_t count) {
  void		*ptr;
  ssize_t	s;
  size_t	available;

  while (count) {
    if (!(ptr = rdfl_b_buffer_getchunk_extend(&obj->data, &available,
	    (RDFL_OPT_ISSET(obj->settings, RDFL_ADJUST_BUFFSIZE)
	     ? count : (size_t)obj->v.buffsize))))
      return (ERR_MEMORY);
    if (available > count) available = count;
    if ((s = rdfl_b_push_read(&obj->data, obj->fd, ptr, available)) < 0)
      return (s);
    count -= (size_t)s;
  }
  return (ERR_NONE);
}

static
ssize_t
_read_all_available(t_rdfl *obj) {
  size_t	available, total = 0;
  ssize_t	full = 1;
  void		*ptr;

  while (full) {
    if (!(ptr = rdfl_b_buffer_getchunk_extend(&obj->data, &available, obj->v.buffsize)))
      return (ERR_MEMORY);
    if ((full = rdfl_b_push_read(&obj->data, obj->fd, ptr, available)) < 0)
      return (full);
    total += full;
    full = ((size_t)full == available);
  }
  return (total);
}

static
size_t
_read_noextend(t_rdfl *obj, size_t consume, e_rdflerrors *err) {
  size_t	s, total;
  ssize_t	ret;
  void		*ptr;

  rdfl_b_consume_size(&obj->data, consume);
  ptr = rdfl_b_buffer_getchunk(&obj->data, &s);
  if (!s) { if (err) *err = ERR_NOSPACELEFT; return (0); }
  if ((ret = rdfl_b_push_read(&obj->data, obj->fd, ptr, s)) < 0) {
    if (err) *err = ret;
    return (0);
  }
  total = (size_t)ret;
  if (RDFL_OPT_ISSET(obj->settings, RDFL_FILLFREESPACE) && total == s) {
    ptr = rdfl_b_buffer_getchunk(&obj->data, &s);
    if (!s) return ((size_t)ret);
    if ((ret = rdfl_b_push_read(&obj->data, obj->fd, ptr, s)) < 0) {
      if (err) *err = ret;
      return (total);
    }
  }
  return ((size_t)ret + total);
}

static
ssize_t
_read_legacy(t_rdfl *obj, void *buf, size_t count) {
  (void)obj, (void)buf, (void)count;
  return (0);
}

static
size_t
_read_monitoring(t_rdfl *obj, e_rdflerrors *err) {
  t_rdfl_net	nw;
  size_t	total;
  int		nw_ret;

  if (err) *err = ERR_NONE;
  if ((nw_ret = rdfl_nw_init(&nw, obj->fd, obj->v.timeout)) < 0) {
    if (err) *err = nw_ret;
    return (0);
  }
  if (RDFL_OPT_ISSET(obj->settings, RDFL_ALL_AVAILABLE))
    total = rdfl_monitoring_allavail(obj, &nw, err);
  else
    total = rdfl_monitoring_read(obj, &nw, err);
  rdfl_nw_clean(&nw);
  return (total);
}

// Destructors
//
void
rdfl_clean(t_rdfl *obj) {
  if (!obj) return ;
  if (RDFL_OPT_ISSET(obj->settings, RDFL_LOC_OPEN))
    close(obj->fd);
  rdfl_buffer_clean(&obj->data);
  if (RDFL_OPT_ISSET(obj->settings, RDFL_LOC_ALLOC))
    free(obj);
}

// Constructors
//
static
int
_check_settings(e_rdflsettings settings) {
  if (RDFL_OPT_ISSET(settings, RDFL_ALL_AVAILABLE)
      && (RDFL_OPT_ISSET(settings, RDFL_NO_EXTEND)
	|| RDFL_OPT_ISSET(settings, RDFL_FILLFREESPACE))) {
    return (EXIT_FAILURE);
  } // Cannot read ALL_AVAILABLE with constraints from NO_EXTEND
  // Illogic to FILLFREESPACE when already reading ALL_AVAILABLE data
  if (RDFL_OPT_ISSET(settings, RDFL_TIMEOUT)
      && !RDFL_OPT_ISSET(settings, RDFL_MONITORING)) {
    return (EXIT_FAILURE);
  } // Cannot apply a timeout without monitoring the fd
  if (RDFL_OPT_ISSET(settings, RDFL_LEGACY)
      && (RDFL_OPT_ISSET(settings, RDFL_ALL_AVAILABLE)
	|| (RDFL_OPT_ISSET(settings, RDFL_FULLEMPTY)))) {
    return (EXIT_FAILURE);
  } // LEGACY means reading in the user's buffer
  if (RDFL_OPT_ISSET(settings, RDFL_FULLEMPTY)
      && RDFL_OPT_ISSET(settings, RDFL_NO_EXTEND)) {
    return (EXIT_FAILURE);
  } // NO_EXTEND will always have a fixed buffer. Won't free it.
  if (RDFL_OPT_ISSET(settings, RDFL_ADJUST_BUFFSIZE)
      && !RDFL_OPT_ISSET(settings, RDFL_FORCEREADSIZE)) {
    return (EXIT_FAILURE);
  } // can ADJUST_BUFFSIZE only if it knows what you need from FORCEREADSIZE
  return (EXIT_SUCCESS);
}

static
void *
_check_func(e_rdflsettings settings) {
  unsigned int	i = 0;
  static struct {
    void *fct;
    e_rdflsettings	flag;
  }	readersTable[] = {
    {&_read_size, RDFL_FORCEREADSIZE},
    {&_read_monitoring, RDFL_MONITORING},
    {&_read_noextend, RDFL_NO_EXTEND},
    {&_read_all_available, RDFL_ALL_AVAILABLE},
    {&_read_legacy, RDFL_LEGACY},
  };
  while (i < (sizeof(readersTable) / sizeof(*readersTable))) {
    if (RDFL_OPT_ISSET(settings, readersTable[i].flag))
      return (readersTable[i].fct);
    ++i;
  }
  return (&_read_legacy); // TODO single read normal func
}

void *
rdfl_load(t_rdfl *new, int fd, e_rdflsettings settings, e_rdflerrors *err) {
  if (_check_settings(settings) == EXIT_FAILURE) {
    if (err) *err = ERR_BADFLAGS;
    return (NULL);
  }
  new->fd = fd;
  new->settings = settings;
  if (rdfl_buffer_init(&(new->data),
	((RDFL_OPT_ISSET(settings, RDFL_ADJUST_BUFFSIZE)
	  || RDFL_OPT_ISSET(settings, RDFL_FULLEMPTY)) ?
	 0 : new->v.buffsize)) == EXIT_FAILURE) {
    if (err) *err = ERR_MEMORY;
    return (NULL);
  }
  if (err) *err = ERR_NONE;
  return (_check_func(settings));
}

void *
rdfl_load_fileptr(t_rdfl *new, FILE *stream, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd_type;

  if ((fd_type = fileno(stream)) == -1) {
    if (err) *err = ERR_BADF;
    return (NULL);
  }
  return (rdfl_load(new, fd_type, settings, err));
}

void *
rdfl_load_path(t_rdfl *new, const char *path, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd;

  if ((fd = open(path, O_RDONLY)) == - 1) {
    if (err) *err = ERR_OPEN;
    return (NULL);
  }
  RDFL_OPT_SET(settings, RDFL_LOC_OPEN);
  return (rdfl_load(new, fd, settings, err));
}

void *
rdfl_load_connect(t_rdfl *new, const char *ip, int port, e_rdflsettings settings, e_rdflerrors *err) {
  int		fd_sock;

  if ((fd_sock = rdfl_nw_openconnection(ip, port)) < 0) {
    if (err) *err = fd_sock;
    return (NULL);
  }
  RDFL_OPT_SET(settings, RDFL_LOC_OPEN);
  return (rdfl_load(new, fd_sock, settings, err));
}

// Helpers
//
const char *
handler_typedef_declare(void *ptr) {
  unsigned int	i = 0;
  static struct {
    void	*ptr;
    const char	*name;
  } nametypes[] = {
    {&_read_all_available, "readall_handler_t"},
    {&_read_noextend, "readnoextend_handler_t"},
    {&_read_monitoring, "readmonitoring_handler_t"},
    {&_read_size, "readsize_handler_t"},
    {&_read_legacy, "readlegacy_handler_t"}
  };
  while (i < (sizeof(nametypes) / sizeof(*nametypes))) {
    if (ptr == nametypes[i].ptr)
      return (nametypes[i].name);
    ++i;
  }
  return (NULL);
}

void
rdfl_printbufferstate(t_rdfl *obj) {
  rdfl_b_print_buffers(&obj->data);
}

// Opt setters
//
void	rdfl_init(t_rdfl *dest) {
  if (!dest) return ;
  static t_rdfl_values		v = {
    .timeout = RDFL_DEFAULT_TIMEOUT,
    .buffsize = RDFL_DEFAULT_BUFFSIZE,
  };
  memcpy(&(dest->v), &v, sizeof(dest->v));
  dest->settings = RDFL_NONE;
  dest->fd = -1;
}

t_rdfl	*rdfl_init_new(void) {
  t_rdfl	*new = malloc(sizeof(*new));
  if (!new) return (NULL);
  rdfl_init(new);
  RDFL_OPT_SET(new->settings, RDFL_LOC_ALLOC);
  return (new);
}

void rdfl_set_timeout(t_rdfl *r, ssize_t timeout)
{ r->v.timeout = (timeout < -1 ? RDFL_DEFAULT_TIMEOUT : timeout); }
void rdfl_set_buffsize(t_rdfl *r, ssize_t buffsize)
{ r->v.buffsize = ((buffsize == 0) ? RDFL_DEFAULT_BUFFSIZE : buffsize); }
