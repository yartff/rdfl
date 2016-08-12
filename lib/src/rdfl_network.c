#if		__STDC_VERSION__ >= 199901L
# define	_XOPEN_SOURCE	600
#else
# define	_XOPEN_SOURCE	500
#endif		/* !__STDC_VERSION__ */
#include	<time.h>

#include	<netdb.h>
#include	<arpa/inet.h>
#include	<stdlib.h>
#include	<string.h>
#include	"rdfl_local.h"
#include	"rdfl_network.h"

void
rdfl_nw_clean(t_rdfl_net *obj) {
  free(obj->timeout);
}

inline
void
rdfl_nw_hit_timeout(t_rdfl_net *obj, ssize_t timeout) {
  obj->timeout->tv_usec = timeout % 1000000;
  obj->timeout->tv_sec = timeout / 1000000;
}

e_rdflerrors
rdfl_nw_init_timeout(t_rdfl_net *obj, ssize_t timeout) {
  if (timeout == -1) {
    if (obj->timeout) {
      free(obj->timeout);
      obj->timeout = NULL;
    }
  }
  else {
    if (!obj->timeout)
      if (!(obj->timeout = malloc(sizeof(*obj->timeout))))
	return (ERR_MEMORY);
    rdfl_nw_hit_timeout(obj, timeout);
  }
  return (ERR_NONE);
}

void
rdfl_nw_init_select(t_rdfl_net *obj, int fd) {
  FD_ZERO(&obj->fds);
  FD_SET(fd, &obj->fds);
  obj->fd_select = fd + 1;
}

e_rdflerrors
rdfl_nw_init(t_rdfl_net *obj, int fd, ssize_t timeout) {
  rdfl_nw_init_select(obj, fd);
  return (rdfl_nw_init_timeout(obj, timeout));
}

static
nwret_t
rdfl_nw_select(int fd_select, fd_set *cpy,
    struct timeval *to,
    ssize_t (*callback)(void *), void *obj) {
  int		ret;
  nwret_t		val;

  ret = select(fd_select, cpy, NULL, NULL, to);
  if (ret == -1)
    return (ERR_SELECT);
  if (ret == 0)
    return (VAL_TIMEOUT_REACHED);
  val = callback(obj);
  if (!val)
    return (ERR_CONNECTION_CLOSED);
  return (val);
}

nwret_t
rdfl_nw_monitoring(t_rdfl_net *net, ssize_t (*callback)(void *), void *obj) {
  struct timeval	tv, *p;
  fd_set		cpy;

  memcpy(&cpy, &net->fds, sizeof(cpy));
  if (net->timeout) {
    p = &tv;
    memcpy(&tv, net->timeout, sizeof(tv));
  }
  else {
    p = NULL;
  }
  return (rdfl_nw_select(net->fd_select, &cpy, p, callback, obj));
}

#if 0
static
e_rdflerrors
_clock_gettime(struct timespec *ts) {
  if (clock_gettime(CLOCK_MONOTONIC, ts) == -1)
    return (ERR_TIMER);
  return (ERR_NONE);
}

nwret_t
rdfl_nw_timeout(t_rdfl_net *net, ssize_t (*callback)(void *), void *obj, struct timespec *ts) {
  (void)net, (void)callback, (void)obj, (void)ts;
  // TODO
  return (ERR_NONE);
}
#endif

e_rdflerrors
rdfl_nw_openconnection(const char *ip, int port) {
  struct protoent	*ent;
  struct sockaddr_in	sin;
  int			fd_sock;

  // TODO signal(SIGPIPE) ??
  if (!(ent = getprotobyname("TCP"))
      || (fd_sock = socket(AF_INET, SOCK_STREAM, ent->p_proto)) == -1)
    return (ERR_CONNECTION);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  if ((sin.sin_addr.s_addr = inet_addr(ip)) == INADDR_NONE
      || connect(fd_sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1) {
    close(fd_sock);
    return (ERR_CONNECTION);
  }
  return (fd_sock);
}
