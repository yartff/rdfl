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
  // RDFL_CONSUMER_THREAD	= (RDFL_NONE + 1) << 3,
  RDFL_NO_EXTEND	= (RDFL_NONE + 1) << 4,
  // Users must not use it on a socket fd, except with monitoring
  // TODO if monitoring, no timeout (blocking read)
  RDFL_FORCESIZE	= (RDFL_NONE + 1) << 5,
  RDFL_ALL_AVAILABLE	= (RDFL_NONE + 1) << 6,
  RDFL_LEGACY		= (RDFL_NONE + 1) << 7,
  RDFL_THREADSAFE	= (RDFL_NONE + 1) << 8,
  RDFL_MONITORING	= (RDFL_NONE + 1) << 9,
  // RDFL_KEEPTRACK	= (RDFL_NONE + 1) << 10, Keep line/col infos
  // RDFL_SIGMASK	= (RDFL_NONE + 1) << 11, // Use pselect instead
  RDFL_FULLEMPTY	= (RDFL_NONE + 1) << 12,
  RDFL_LAST		= (RDFL_NONE + 1) << 13,
}			e_rdflsettings;

typedef			enum {
  ERR_NONE		= 0,
  ERR_SIZETOOBIG	= -1,
  ERR_READ		= -2,
  ERR_MEMORY		= -3,
  ERR_BADF		= -4,
  ERR_BADFLAGS		= -5,
  ERR_NOSPACELEFT	= -6,
  ERR_OPEN		= -7,
}			e_rdflerrors;

typedef			struct {
  ssize_t		timeout;
  ssize_t		buffsize;
}			t_rdfl_values;

typedef			struct {
  t_rdfl_values		v;
  e_rdflsettings	settings;
  int			fd;
  t_rdfl_buffer		data;
}			t_rdfl;

typedef	ssize_t		(*readall_handler_t)(t_rdfl *);
typedef	ssize_t		(*readnoextend_handler_t)(t_rdfl *, size_t consume);
typedef	ssize_t		(*readlegacy_handler_t)(t_rdfl *, void *, size_t);
typedef	ssize_t		(*readmonitoring_handler_t)(t_rdfl *);
typedef	ssize_t		(*readsize_handler_t)(t_rdfl *, size_t s);

// API
//

// Init data
void		rdfl_init(t_rdfl *);
t_rdfl		*rdfl_init_new(void);

// Launch the stream
void		*rdfl_load(t_rdfl *, int fd, e_rdflsettings, e_rdflerrors *);
void		*rdfl_load_fileptr(t_rdfl *, FILE *file_ptr, e_rdflsettings, e_rdflerrors *);
void		*rdfl_load_path(t_rdfl *, const char *path, e_rdflsettings, e_rdflerrors *);

const char	*handler_typedef_declare(void *ptr);
void		rdfl_clean(t_rdfl *obj);

void		rdfl_set_timeout(t_rdfl *, ssize_t timeout);
void		rdfl_set_buffsize(t_rdfl *, ssize_t buffsize);
void		*rdfl_flush_buffers_alloc(t_rdfl *obj, ssize_t *count_value);
void		*rdfl_getinplace_next_chunk(t_rdfl *, size_t *, size_t *);

#endif			/* !__RDFL_H_ */
