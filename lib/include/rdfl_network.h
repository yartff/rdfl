#ifndef			__RDFL_NETWORK_H_
# define		__RDFL_NETWORK_H_

# include		<sys/select.h>
# include		<sys/time.h>
# include		<sys/types.h>

typedef			struct {
  struct timeval	*timeout;
  fd_set		fds;
  int			fd_select;
}			t_rdfl_net;

typedef	ssize_t		nwret_t;

e_rdflerrors		rdfl_nw_init_timeout(t_rdfl_net *, ssize_t);
e_rdflerrors		rdfl_nw_init(t_rdfl_net *, int, ssize_t);
void			rdfl_nw_init_select(t_rdfl_net *, int);
void			rdfl_nw_clean(t_rdfl_net *);
nwret_t			rdfl_nw_monitoring(t_rdfl_net *, ssize_t (*callback)(void *), void *);
e_rdflerrors		rdfl_nw_openconnection(const char *ip, int port);

#endif			/* !__RDFL_NETWORK_H_ */
