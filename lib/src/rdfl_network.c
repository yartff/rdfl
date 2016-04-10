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

int
rdfl_nw_init(t_rdfl_net *obj, int fd, ssize_t timeout) {
  if (timeout == -1)
    obj->timeout = NULL;
  else {
    if (!(obj->timeout = malloc(sizeof(*obj->timeout))))
      return (ERR_MEMORY);
    obj->timeout->tv_usec = timeout % 1000000;
    obj->timeout->tv_sec = timeout / 1000000;
  }
  FD_ZERO(&obj->fds);
  FD_SET(fd, &obj->fds);
  obj->fd_select = fd + 1;
  return (ERR_NONE);
}

int
rdfl_nw_monitoring(t_rdfl_net *net) {
  struct timeval	tv, *p;
  fd_set		cpy;
  int			ret;

  memcpy(&cpy, &net->fds, sizeof(cpy));
  if (net->timeout) {
    p = &tv;
    memcpy(&tv, net->timeout, sizeof(tv));
  }
  else {
    p = NULL;
  }
  return (((ret = select(net->fd_select, &cpy, NULL, NULL, p)) == -1)
      ? ERR_SELECT : ret);
}

int
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
