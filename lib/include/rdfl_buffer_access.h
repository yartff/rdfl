#ifndef			__RDFL_BUFFER_ACCESS_H_
# define		__RDFL_BUFFER_ACCESS_H_

# include		"rdfl.h"

void		*rdfl_bacc_getcontent(t_rdfl *, size_t *, size_t, e_bacc_options);
int		rdfl_bacc_cmp(t_rdfl *, void *, size_t);
int		rdfl_bacc_readptr(t_rdfl *, void *, size_t, e_bacc_options);
size_t		rdfl_bacc_info_total(t_rdfl *);
int		rdfl_bacc_ndx(t_rdfl *, size_t);

// TODO not public
int		_iterate_chunk(t_rdfl *, int (*)(void *, size_t, void *), void *, e_bacc_options opt);
int		_iterate_extract(t_rdfl *, void **, ssize_t, e_bacc_options);
#endif			/* !__RDFL_BUFFER_ACCESS_H_ */
