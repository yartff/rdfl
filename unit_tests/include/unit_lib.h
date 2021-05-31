#ifndef		__UNIT_LIB_H_
# define	__UNIT_LIB_H_

# include	"rdfl.h"
# include	"rdfl_devel.h"
# include	"unit_internal.h"

e_unit_return_code			init_lib();
e_unit_return_code			lib_load(e_unit_log_type);
void					*lib_load_function(const char *, LOGP);
void					clean_rdfl(void);

e_buildMode				_get_buildMode_opt(void);
e_unit_return_code			_check_buildMode(e_buildMode, LOGP);
e_unit_return_code			_build_rdfl(e_buildMode mode, LOGP);


/*
 * Public functions
 */
extern readsize_handler_t		l__rdflReader_size;
extern readall_handler_t		l__rdflReader_allavail;
extern readnoextend_handler_t		l__rdflReader_noextend;
extern readlegacy_handler_t		l__rdflReader_legacy;
extern readmonitoring_handler_t		l__rdflReader_monitoring;
extern readmonitoringnoext_handler_t	l__rdflReader_monitoring_noextend;
extern readmonitoringall_handler_t	l__rdflReader_monitoring_allavail;
extern readsinglestep_t			l__rdflReader_singlestep;

extern void				(*l__rdfl_init)(t_rdfl *);
extern t_rdfl				*(*l__rdfl_init_new)(void);

extern int				(*l__rdfl_set_timeout)(t_rdfl *, ssize_t);
extern void				(*l__rdfl_set_buffsize)(t_rdfl *, ssize_t);
extern int				(*l__rdfl_add_comment)(t_rdfl *, const char *begin, const char *end);

extern int				(*l__rdfl_load)(t_rdfl *, int, e_rdflsettings, e_rdflerrors *);
extern int				(*l__rdfl_load_fileptr)(t_rdfl *, FILE *, e_rdflsettings, e_rdflerrors *);
extern int				(*l__rdfl_load_path)(t_rdfl *, const char *, e_rdflsettings, e_rdflerrors *);
extern int				(*l__rdfl_load_connect)(t_rdfl *, const char *, int, e_rdflsettings, e_rdflerrors *);

extern rdfl_retval			(*l__rdfl_clean)(t_rdfl *);

extern int				(*l__rdfl_eofreached)(t_rdfl *);
extern void				(*l__rdfl_consume_size)(t_rdfl *, size_t);

// DEVEL methods
extern void				(*l__rdflDevel_printbufferstate)(t_rdfl *obj);
extern void				(*l__rdflDevel_get_func)(e_rdflsettings);
extern const char			*(*l__rdflDevel_handler_typedef_declare)(void *ptr);
extern const char			*(*l__rdflDevel_handler_func_declare)(void *ptr);
/**/

extern e_buildMode			(*l__rdflBuild_getMode)(void);

#endif		/* !__UNIT_LIB_H_ */
