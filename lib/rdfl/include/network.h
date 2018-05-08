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

e_rdflerrors		nw_init_timeout(t_rdfl_net *, ssize_t);
e_rdflerrors		nw_init(t_rdfl_net *, int, ssize_t);
void			nw_clean(t_rdfl_net *);
nwret_t			nw_monitoring(t_rdfl_net *, ssize_t (*callback)(void *), void *);
e_rdflerrors		nw_openconnection(const char *ip, int port);

#endif			/* !__RDFL_NETWORK_H_ */
