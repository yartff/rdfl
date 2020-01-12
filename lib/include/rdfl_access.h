#ifndef		__RDFL_ACCESS_H_
# define	__RDFL_ACCESS_H_

# include	"rdfl_types.h"

void		*rdfl_acc_getcontent(t_rdfl *, ssize_t *, size_t c, e_acc_options);
int		rdfl_acc_cmp(t_rdfl *, void *, size_t, e_acc_options);
size_t		rdfl_acc_info_total(t_rdfl *);
int		rdfl_acc_ndx(t_rdfl *, size_t);

#endif		/* !__RDFL_ACCESS_H_ */
