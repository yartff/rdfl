#ifndef			__RDFL_H_
# define		__RDFL_H_

# include		<stdio.h>
# include		"rdfl_types.h"

//// rdfl.c
// Init data
void			rdfl_init(t_rdfl *);
t_rdfl			*rdfl_init_new(void);

// Options
int			rdfl_set_timeout(t_rdfl *, ssize_t timeout);
void			rdfl_set_buffsize(t_rdfl *, ssize_t buffsize);
int			rdfl_add_comment(t_rdfl *, const char *begin, const char *end);

// Load input. (EXIT_SUCCESS, EXIT_FAILURE)
int			rdfl_load(t_rdfl *, int fd, e_rdflsettings, e_rdflerrors *);
int			rdfl_load_fileptr(t_rdfl *, FILE *file_ptr, e_rdflsettings, e_rdflerrors *);
int			rdfl_load_path(t_rdfl *, const char *path, e_rdflsettings, e_rdflerrors *);
int			rdfl_load_connect(t_rdfl *, const char *, int, e_rdflsettings, e_rdflerrors *);

// Destructor
rdfl_retval		rdfl_clean(t_rdfl *obj);

//// rdfl_readers.c
// Readers
typedef	ssize_t		(*readsize_handler_t)(t_rdfl *, size_t s);
ssize_t			rdflReader_size(t_rdfl *, size_t);

typedef	ssize_t		(*readall_handler_t)(t_rdfl *, e_rdflerrors *);
ssize_t			rdflReader_allavail(t_rdfl *, e_rdflerrors *);

typedef	ssize_t		(*readnoextend_handler_t)(t_rdfl *, size_t consume);
ssize_t			rdflReader_noextend(t_rdfl *, size_t consume);

typedef	ssize_t		(*readlegacy_handler_t)(t_rdfl *, void *, size_t);
ssize_t			rdflReader_legacy(t_rdfl *, void *, size_t);

typedef	ssize_t		(*readmonitoring_handler_t)(t_rdfl *);
ssize_t			rdflReader_monitoring(t_rdfl *);

typedef ssize_t		(*readmonitoringnoext_handler_t)(t_rdfl *);
ssize_t			rdflReader_monitoring_noextend(t_rdfl *);

typedef ssize_t		(*readmonitoringall_handler_t)(t_rdfl *, e_rdflerrors *);
ssize_t			rdflReader_monitoring_allavail(t_rdfl *, e_rdflerrors *);

typedef	ssize_t		(*readsinglestep_t)(t_rdfl *);
ssize_t			rdflReader_singlestep(t_rdfl *);

//// rdfl_utils.c
// Utils
int			rdfl_eofreached(t_rdfl *obj);
void			rdfl_consume_size(t_rdfl *obj, size_t s);
void			*rdfl_getinplace_next_chunk(t_rdfl *, size_t *, size_t *);
// TODO: rdfl_inputuserdata(t_rdfl *obj, void *, size_t s);

//// rdfl_infos.c
// Infos
// size_t			rdfl_get_total_datasize(t_rdfl *obj);

#endif			/* !__RDFL_H_ */
