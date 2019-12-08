#ifndef		__ITERATE_H_
# define	__ITERATE_H_

# include	"rdfl_types.h"

int		_iterate_chunk(t_rdfl *, int (*)(void *, size_t, void *), void *, e_bacc_options opt);
int		_iterate_extract(t_rdfl *, void **, ssize_t, e_bacc_options);

#endif		/* !__ITERATE_H_ */
