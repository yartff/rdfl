#include	"unit_public.h"
#include	"unit_devel.h"
#include	"unit_lib.h"

readsize_handler_t		l__rdflReader_size;
readall_handler_t		l__rdflReader_allavail;
readnoextend_handler_t		l__rdflReader_noextend;
readlegacy_handler_t		l__rdflReader_legacy;
readmonitoring_handler_t	l__rdflReader_monitoring;
readmonitoringnoext_handler_t	l__rdflReader_monitoring_noextend;
readmonitoringall_handler_t	l__rdflReader_monitoring_allavail;
readsinglestep_t		l__rdflReader_singlestep;

void				(*l__rdfl_init)(t_rdfl *);
t_rdfl				*(*l__rdfl_init_new)(void);

int				(*l__rdfl_set_timeout)(t_rdfl *, ssize_t);
void				(*l__rdfl_set_buffsize)(t_rdfl *, ssize_t);
int				(*l__rdfl_add_comment)(t_rdfl *, const char *begin, const char *end);

int				(*l__rdfl_load)(t_rdfl *, int, e_rdflsettings, e_rdflerrors *);
int				(*l__rdfl_load_fileptr)(t_rdfl *, FILE *, e_rdflsettings, e_rdflerrors *);
int				(*l__rdfl_load_path)(t_rdfl *, const char *, e_rdflsettings, e_rdflerrors *);
int				(*l__rdfl_load_connect)(t_rdfl *, const char *, int, e_rdflsettings, e_rdflerrors *);

rdfl_retval			(*l__rdfl_clean)(t_rdfl *);

int				(*l__rdfl_eofreached)(t_rdfl *);
void				(*l__rdfl_consume_size)(t_rdfl *, size_t);

#ifdef DEVEL
void				(*l__rdflDevel_printbufferstate)(t_rdfl *obj);
void				(*l__rdflDevel_get_func)(e_rdflsettings);
const char			*(*l__rdflDevel_handler_typedef_declare)(void *ptr);
const char			*(*l__rdflDevel_handler_func_declare)(void *ptr);
#endif

void
load_public_functions(void) {
  l__rdflReader_size			= load_rdfl_function("rdflReader_size", 1);
  l__rdflReader_allavail		= load_rdfl_function("rdflReader_allavail", 1);
  l__rdflReader_noextend		= load_rdfl_function("rdflReader_noextend", 1);
  l__rdflReader_legacy			= load_rdfl_function("rdflReader_legacy", 1);
  l__rdflReader_monitoring		= load_rdfl_function("rdflReader_monitoring", 1);
  l__rdflReader_monitoring_noextend	= load_rdfl_function("rdflReader_monitoring_noextend", 1);
  l__rdflReader_monitoring_allavail	= load_rdfl_function("rdflReader_monitoring_allavail", 1);
  l__rdflReader_singlestep		= load_rdfl_function("rdflReader_singlestep", 1);

  l__rdfl_init				= load_rdfl_function("rdfl_init", 1);
  l__rdfl_init_new			= load_rdfl_function("rdfl_init_new", 1);

  l__rdfl_set_timeout			= load_rdfl_function("rdfl_set_timeout", 1);
  l__rdfl_set_buffsize			= load_rdfl_function("rdfl_set_buffsize", 1);
  l__rdfl_add_comment			= load_rdfl_function("rdfl_add_comment", 1);

  l__rdfl_load				= load_rdfl_function("rdfl_load", 1);
  l__rdfl_load_fileptr			= load_rdfl_function("rdfl_load_fileptr", 1);
  l__rdfl_load_path			= load_rdfl_function("rdfl_load_path", 1);
  l__rdfl_load_connect			= load_rdfl_function("rdfl_load_connect", 1);

  l__rdfl_clean				= load_rdfl_function("rdfl_clean", 1);

  l__rdfl_eofreached			= load_rdfl_function("rdfl_eofreached", 1);
  l__rdfl_consume_size			= load_rdfl_function("rdfl_consume_size", 1);

#ifdef DEVEL
  l__rdflDevel_printbufferstate		= load_rdfl_function("rdflDevel_printbufferstate", 1);
  l__rdflDevel_get_func			= load_rdfl_function("rdflDevel_get_func", 1);
  l__rdflDevel_handler_typedef_declare	= load_rdfl_function("rdflDevel_handler_typedef_declare", 1);
  l__rdflDevel_handler_func_declare	= load_rdfl_function("rdflDevel_handler_func_declare", 1);
#endif
}
