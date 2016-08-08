#ifndef			__RDFL_H_
# define		__RDFL_H_

# include		<stdio.h>
# include		<unistd.h>
# include		"rdfl_buffer.h"
# include		"rdfl_network.h"
# include		"rdfl_status_codes.h"

# define		RDFL_OPT_ISSET(value, opt)	(value & opt)
# define		RDFL_OPT_SET(value, opt)	(value |= opt)
# define		RDFL_OPT_UNSET(value, opt)	((RDFL_OPT_ISSET(value, opt) ? value : value ^= opt))
# define		RDFL_OPT_CONTAINALL(value, opt)	((value & opt) == opt)

typedef			struct {
  ssize_t		timeout;
  ssize_t		buffsize;
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
int			_read_size(t_rdfl *, size_t);

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

const char	*handler_typedef_declare(void *ptr);
void		*get_func(e_rdflsettings);
void		rdfl_clean(t_rdfl *obj);

int		rdfl_set_timeout(t_rdfl *, ssize_t timeout);
void		rdfl_set_buffsize(t_rdfl *, ssize_t buffsize);
void		*rdfl_flush_buffers_alloc(t_rdfl *obj, ssize_t *count_value);
void		*rdfl_getinplace_next_chunk(t_rdfl *, size_t *, size_t *);
void		rdfl_force_consume_size(t_rdfl *obj, size_t s);
void		rdfl_printbufferstate(t_rdfl *obj);

#endif			/* !__RDFL_H_ */
