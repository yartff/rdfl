#ifndef			__RDFL_H_
# define		__RDFL_H_

# include		<stdio.h>
# include		<unistd.h>
# include		"rdfl_buffer.h"

# define		RDFL_OPT_ISSET(value, opt)	(value & opt)
# define		RDFL_OPT_SET(value, opt)	(value |= opt)
# define		RDFL_OPT_UNSET(value, opt)	\
  ((RDFL_OPT_ISSET(value, opt) ? value : value ^= opt))

typedef			enum {
  RDFL_NONE		= 0,
  RDFL_INPLACE		= (RDFL_NONE + 1) << 1,
  RDFL_TIMEOUT		= (RDFL_NONE + 1) << 2,
  // vvv : will come with consumer feature (ran in a different thread)
  RDFL_CONSUMER_THREAD	= (RDFL_NONE + 1) << 3,
  RDFL_NO_EXTEND	= (RDFL_NONE + 1) << 4,
  RDFL_ALL_AVAILABLE	= (RDFL_NONE + 1) << 5,
  RDFL_THREADSAFE	= (RDFL_NONE + 1) << 5,
  // RDFL_SIGMASK	= (RDFL_NONE + 1) << 6, // Use pselect instead
  RDFL_LAST		= (RDFL_NONE + 1) << 7,
}			e_rdflsettings;

typedef			struct {
  ssize_t		timeout;
  size_t		buffsize;
}			t_rdfl_values;

typedef			struct {
  t_rdfl_values		v;
  e_rdflsettings	settings;
  int			fd;
  t_rdfl_buffer		data;
}			t_rdfl;

typedef	void		*(*readall_handler_t)(t_rdfl *, ssize_t *);
typedef	void		*(*readinpl_handler_t)(t_rdfl *, ssize_t *, size_t consumed);
typedef	ssize_t		(*readlegacy_handler_t)(t_rdfl *, void *, size_t);

// API
//
void		rdfl_init(t_rdfl *);
void		*rdfl_load(t_rdfl *, int fd, e_rdflsettings);
void		*rdfl_load_fileptr(t_rdfl *, FILE *file_ptr, e_rdflsettings);
void		*rdfl_load_path(t_rdfl *, const char *path, e_rdflsettings);
const char	*handler_typedef_declare(void *ptr);

void		rdfl_set_timeout(t_rdfl *, ssize_t timeout);
void		rdfl_set_buffsize(t_rdfl *, size_t buffsize);

#endif			/* !__RDFL_H_ */
