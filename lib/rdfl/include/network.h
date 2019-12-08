#ifndef			__NETWORK_H_
# define		__NETWORK_H_

# include		"rdfl_types.h"

e_rdflerrors		nw_init_timeout(t_rdfl_net *, ssize_t);
e_rdflerrors		nw_init(t_rdfl_net *, int, ssize_t);
void			nw_clean(t_rdfl_net *);
nwret_t			nw_monitoring(t_rdfl_net *, ssize_t (*callback)(void *), void *);
e_rdflerrors		nw_openconnection(const char *ip, int port);

#endif			/* !__NETWORK_H_ */
