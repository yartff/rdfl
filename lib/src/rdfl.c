#include	<fcntl.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	"rdfl_local.h"

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
	     ? count : obj->v.buffsize))))
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

  // TODO monitoring
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
ssize_t
_read_noextend(t_rdfl *obj, size_t consume) {
  size_t	s;
  ssize_t	ret;
  void		*ptr;

  rdfl_b_consume_size(&obj->data, consume);
  ptr = rdfl_b_buffer_getchunk(&obj->data, &s);
  if (!s) return (ERR_NOSPACELEFT);
  if ((ret = rdfl_b_push_read(&obj->data, obj->fd, ptr, s)) < 0) {
    return (ret);
  }
  return (ret);
  // TODO read more if available
}

static
ssize_t
_read_legacy(t_rdfl *obj, void *buf, size_t count) {
  (void)obj, (void)buf, (void)count;
  return (0);
}

static
ssize_t
_read_monitoring(t_rdfl *obj) {
  if (RDFL_OPT_ISSET(obj->settings, RDFL_ALL_AVAILABLE)) {
    /*
       return (rdfl_b_push_all_local_monitoring(&obj->data, obj->fd, obj->v.buffsize,
       RDFL_OPT_ISSET(obj->settings, RDFL_TIMEOUT) ? obj->v.timeout : -1));
       */ // TODO
  }
  if (RDFL_OPT_ISSET(obj->settings, RDFL_NO_EXTEND)) {
    // TODO
  }
  return (-1);
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
      && (RDFL_OPT_ISSET(settings, RDFL_NO_EXTEND))) {
    return (EXIT_FAILURE);
  } // Cannot read ALL_AVAILABLE with constraints from NO_EXTEND
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
  return (EXIT_SUCCESS);
}

static
void *
_check_func(e_rdflsettings settings) {
  if (RDFL_OPT_ISSET(settings, RDFL_FORCEREADSIZE))
    return (&_read_size);
  if (RDFL_OPT_ISSET(settings, RDFL_MONITORING))
    return (&_read_monitoring);
  if (RDFL_OPT_ISSET(settings, RDFL_NO_EXTEND))
    return (&_read_noextend);
  if (RDFL_OPT_ISSET(settings, RDFL_ALL_AVAILABLE))
    return (&_read_all_available);
  return (&_read_legacy);
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
rdfl_connect(t_rdfl *new, const char *ip, int port) {
  // TODO
  (void)new, (void)ip, (void)port;
  return (NULL);
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
