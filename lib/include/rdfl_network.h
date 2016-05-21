#ifndef			__RDFL_NETWORK_H_
# define		__RDFL_NETWORK_H_

# include		<sys/select.h>
# include		<sys/time.h>
# include		<sys/types.h>
# include		<unistd.h>

typedef			struct {
  struct timeval	*timeout;
  fd_set		fds;
  int			fd_select;
}			t_rdfl_net;

int			rdfl_nw_init_timeout(t_rdfl_net *, ssize_t);
int			rdfl_nw_init(t_rdfl_net *, int, ssize_t);
void			rdfl_nw_init_select(t_rdfl_net *, int);
void			rdfl_nw_clean(t_rdfl_net *);
int			rdfl_nw_monitoring(t_rdfl_net *);
int			rdfl_nw_openconnection(const char *ip, int port);

#endif			/* !__RDFL_NETWORK_H_ */
