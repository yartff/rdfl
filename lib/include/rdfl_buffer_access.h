#ifndef			__RDFL_BUFFER_ACCESS_H_
# define		__RDFL_BUFFER_ACCESS_H_

# include		"rdfl.h"

typedef		enum {
  RBA_NONE		= 0,
  RBA_NULLTERMINATED	= 1 << 0,
}		e_bacc_options;

void		*rdfl_bacc_getallcontent(t_rdfl *, size_t *, e_bacc_options);
int		rdfl_bacc_cmp(t_rdfl *, void *, size_t);
int		rdfl_bacc_readptr(t_rdfl *, void *, size_t);
size_t		rdfl_bacc_info_total(t_rdfl *);
int		rdfl_bacc_ndx(t_rdfl *, size_t);

#endif			/* !__RDFL_BUFFER_ACCESS_H_ */
