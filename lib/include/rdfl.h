#ifndef			__RDFL_H_
# define		__RDFL_H_

# include		<stdio.h>
# include		"rdfl_status_codes.h"
# include		"rdfl_buffer.h"
# include		"rdfl_network.h"

# define		RDFL_OPT_CANCEL(value, opt)	(value & ~opt)
# define		RDFL_OPT_CONTAINALL(value, opt)	((value & opt) == opt)
# define		RDFL_OPT_ISSET(value, opt)	(value & opt)
// Don't change settings at runtime. No checks made for optimization purposes
# define		RDFL_OPT_SET(value, opt)	(value |= opt)

// Can't use this one with sevel opts.
# define		RDFL_OPT_UNSET(value, opt)	\
  do { \
    if (RDFL_OPT_ISSET(value, opt)) value ^= opt; \
  } while (0);

typedef struct		s_comments {
  char			*beg;
  char			*end;
  struct s_comments	*next;
}			t_comments;

typedef			struct {
  ssize_t		timeout;
  ssize_t		buffsize;
  t_comments		*cmts;
}			t_rdfl_values;

typedef			struct {
  t_rdfl_net		*nw;
  t_rdfl_values		v;
  e_rdflsettings	settings;
  int			fd;
  t_rdfl_buffer		data;
}			t_rdfl;

// Reader Fct
typedef	ssize_t		(*readsize_handler_t)(t_rdfl *, size_t s);
ssize_t			_read_size(t_rdfl *, size_t);

typedef	ssize_t		(*readall_handler_t)(t_rdfl *, e_rdflerrors *);
ssize_t			_read_all_available(t_rdfl *, e_rdflerrors *);

typedef	ssize_t		(*readnoextend_handler_t)(t_rdfl *, size_t consume);
ssize_t			_read_noextend(t_rdfl *, size_t consume);

typedef	ssize_t		(*readlegacy_handler_t)(t_rdfl *, void *, size_t);
ssize_t			_read_legacy(t_rdfl *, void *, size_t);

typedef	ssize_t		(*readmonitoring_handler_t)(t_rdfl *);
ssize_t			_read_monitoring(t_rdfl *);

typedef ssize_t		(*readmonitoringnoext_handler_t)(t_rdfl *);
ssize_t			_read_monitoring_no_extend(t_rdfl *);

typedef ssize_t		(*readmonitoringall_handler_t)(t_rdfl *, e_rdflerrors *);
ssize_t			_read_monitoring_allavail(t_rdfl *, e_rdflerrors *);

typedef	ssize_t		(*read_singlestep_t)(t_rdfl *);
ssize_t			_read_singlestep(t_rdfl *);

// API
//

// Init data
void		rdfl_init(t_rdfl *);
t_rdfl		*rdfl_init_new(void);

// Launch the stream
typedef		int	rdflret_t;
rdflret_t	rdfl_load(t_rdfl *, int fd, e_rdflsettings, e_rdflerrors *);
rdflret_t	rdfl_load_fileptr(t_rdfl *, FILE *file_ptr, e_rdflsettings, e_rdflerrors *);
rdflret_t	rdfl_load_path(t_rdfl *, const char *path, e_rdflsettings, e_rdflerrors *);
rdflret_t	rdfl_load_connect(t_rdfl *, const char *, int, e_rdflsettings, e_rdflerrors *);
# ifndef	DEVEL
void		rdfl_clean(t_rdfl *obj);
# endif

int		rdfl_eofreached(t_rdfl *obj);

int		rdfl_set_timeout(t_rdfl *, ssize_t timeout);
void		rdfl_set_buffsize(t_rdfl *, ssize_t buffsize);
int		rdfl_set_skip(t_rdfl *, size_t value);
void		*rdfl_flush_buffers_alloc(t_rdfl *obj, ssize_t *count_value);
void		*rdfl_getinplace_next_chunk(t_rdfl *, size_t *, size_t *);
void		rdfl_force_consume_size(t_rdfl *obj, size_t s);

int		rdfl_add_comment(t_rdfl *, const char *, const char *);
// TODO rdfl_inputuserdata(t_rdfl *obj, void *, size_t s);

#endif			/* !__RDFL_H_ */
