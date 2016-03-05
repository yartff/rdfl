#include	<fcntl.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	"rdfl_local.h"

// API
//
static
void *
_read_all_available(t_rdfl *obj, ssize_t *count_value) {
  (void)count_value, (void)obj;
  return (NULL);
}

static
void *
_read_inplace(t_rdfl *obj, ssize_t *count_value, size_t consumed) {
  (void)obj, (void)count_value, (void)consumed;
  return (NULL);
}

static
ssize_t
_read_legacy(t_rdfl *obj, void *buf, size_t count) {
  (void)obj, (void)buf, (void)count;
  return (0);
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
  if (RDFL_OPT_ISSET(settings, RDFL_ALL_AVAILABLE) &&
      (RDFL_OPT_ISSET(settings, RDFL_INPLACE) || RDFL_OPT_ISSET(settings, RDFL_NO_EXTEND))) {
    return (EXIT_FAILURE);
  } // Cannot read ALL_AVAILABLE with constraints from INPLACE or/and NO_EXTEND
  return (EXIT_SUCCESS);
}

static
void *
_check_func(e_rdflsettings settings) {
  if (RDFL_OPT_ISSET(settings, RDFL_INPLACE))
    return (&_read_inplace);
  if (RDFL_OPT_ISSET(settings, RDFL_ALL_AVAILABLE))
    return (&_read_all_available);
  return (&_read_legacy);
}

void *
rdfl_load(t_rdfl *new, int fd, e_rdflsettings settings) {
  size_t		amount;

  if (_check_settings(settings) == EXIT_FAILURE)
    return (NULL);
  new->fd = fd;
  new->settings = settings;
  amount = (RDFL_OPT_ISSET(settings, RDFL_NO_EXTEND) ? new->v.buffsize : 0);
  if (rdfl_buffer_init(&(new->data), amount) == EXIT_FAILURE)
    return (NULL);
  return (_check_func(settings));
}

void *
rdfl_load_fileptr(t_rdfl *new, FILE *stream, e_rdflsettings settings) {
  int		fd_type;

  if ((fd_type = fileno(stream)) == -1)
    return (NULL);
  return (rdfl_load(new, fd_type, settings));
}

void *
rdfl_load_path(t_rdfl *new, const char *path, e_rdflsettings settings) {
  int		fd;

  if ((fd = open(path, O_RDONLY)) == - 1)
    return (NULL);
  RDFL_OPT_SET(settings, RDFL_LOC_OPEN);
  return (rdfl_load(new, fd, settings));
}

void *
rdfl_connect(t_rdfl *new, const char *ip, int port) {
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
    {&_read_inplace, "readinpl_handler_t"},
    {&_read_legacy, "readlegacy_handler_t"}
  };
  while (i < (sizeof(nametypes) / sizeof(*nametypes))) {
    if (ptr == nametypes[i].ptr)
      return (nametypes[i].name);
    ++i;
  }
  return (NULL);
}

// Opt setters
//
void	rdfl_init(t_rdfl *dest) {
  if (!dest) return ;
  static t_rdfl_values		v = {
    .timeout = RDFL_DEFAULT_TIMEOUT,
    .buffsize = RDFL_DEFAULT_BUFFSIZE,
  };
  memset(dest, 0, sizeof(*dest));
  memcpy(&(dest->v), &v, sizeof(dest->v));
  dest->settings = RDFL_NONE;
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
void rdfl_set_buffsize(t_rdfl *r, size_t buffsize)
{ r->v.buffsize = ((buffsize == 0) ? RDFL_DEFAULT_BUFFSIZE : buffsize); }
