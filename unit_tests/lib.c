#include	<stdlib.h>
#include	<stdio.h>
#include	<dlfcn.h>
#include	"unit_internal.h"
#include	"unit_lib.h"
#include	"unit_opt.h"

static void			*handle = NULL;
static int			isLoaded = 0;

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

// BUILD methods
e_buildMode			(*l__rdflBuild_getMode)(void);

// DEVEL methods
void				(*l__rdflDevel_printbufferstate)(t_rdfl *obj);
void				(*l__rdflDevel_get_func)(e_rdflsettings);
const char			*(*l__rdflDevel_handler_typedef_declare)(void *ptr);
const char			*(*l__rdflDevel_handler_func_declare)(void *ptr);

static
e_unit_return_code
_load_public_functions(LOGP) {
  if (!(l__rdflReader_size			= lib_load_function("rdflReader_size", _logt))
  || !(l__rdflReader_allavail			= lib_load_function("rdflReader_allavail", _logt))
  || !(l__rdflReader_noextend			= lib_load_function("rdflReader_noextend", _logt))
  || !(l__rdflReader_legacy			= lib_load_function("rdflReader_legacy", _logt))
  || !(l__rdflReader_monitoring			= lib_load_function("rdflReader_monitoring", _logt))
  || !(l__rdflReader_monitoring_noextend	= lib_load_function("rdflReader_monitoring_noextend", _logt))
  || !(l__rdflReader_monitoring_allavail	= lib_load_function("rdflReader_monitoring_allavail", _logt))
  || !(l__rdflReader_singlestep			= lib_load_function("rdflReader_singlestep", _logt))

  || !(l__rdfl_init				= lib_load_function("rdfl_init", _logt))
  || !(l__rdfl_init_new				= lib_load_function("rdfl_init_new", _logt))

  || !(l__rdfl_set_timeout			= lib_load_function("rdfl_set_timeout", _logt))
  || !(l__rdfl_set_buffsize			= lib_load_function("rdfl_set_buffsize", _logt))
  || !(l__rdfl_add_comment			= lib_load_function("rdfl_add_comment", _logt))

  || !(l__rdfl_load				= lib_load_function("rdfl_load", _logt))
  || !(l__rdfl_load_fileptr			= lib_load_function("rdfl_load_fileptr", _logt))
  || !(l__rdfl_load_path			= lib_load_function("rdfl_load_path", _logt))
  || !(l__rdfl_load_connect			= lib_load_function("rdfl_load_connect", _logt))

  || !(l__rdfl_clean				= lib_load_function("rdfl_clean", _logt))

  || !(l__rdfl_eofreached			= lib_load_function("rdfl_eofreached", _logt))
  || !(l__rdfl_consume_size			= lib_load_function("rdfl_consume_size", _logt))
// BUILD methods
  || !(l__rdflBuild_getMode			= lib_load_function("rdflBuild_getMode", _logt))
// DEVEL methods
  || !(l__rdflDevel_printbufferstate		= lib_load_function("rdflDevel_printbufferstate", _logt))
  || !(l__rdflDevel_get_func			= lib_load_function("rdflDevel_get_func", _logt))
  || !(l__rdflDevel_handler_typedef_declare	= lib_load_function("rdflDevel_handler_typedef_declare", _logt))
  || !(l__rdflDevel_handler_func_declare	= lib_load_function("rdflDevel_handler_func_declare", _logt))) {
    return (FAILURE);
  }
  return (SUCCESS);
}

void
clean_rdfl(void) {
  if (handle) dlclose(handle);
  handle = NULL;
}

e_unit_return_code
lib_load(LOGP) {
  if (!(handle = dlopen("./librdfl.so", RTLD_LAZY))) {
    LOG_T(dlerror());
    return (FAILURE);
  }
  if (_load_public_functions(_logt)) {
    clean_rdfl();
    return (FAILURE);
  }
  return (SUCCESS);
}

void *
lib_load_function(const char *fname, LOGP) {
  void	*function;
  if (!handle) {
    LOG_T("lib is not loaded");
    return (NULL);
  }
  if (!(function = dlsym(handle, fname))) LOG_T(dlerror());
  return (function);
}

e_unit_return_code
init_lib() {
  e_buildMode	mode = _get_buildMode_opt();
  // TODO export opt check to unit_opt at startup
  if (mode == -1) {
    opt_helpopt('m');
    return (FAILURE);
  }
  if (lib_load(LOGT_LOG) || _check_buildMode(mode, LOGT_LOG)) {
    clean_rdfl();
    _build_rdfl(mode, LOGT_FATAL);
    lib_load(LOGT_FATAL);
    _check_buildMode(mode, LOGT_FATAL);
  }
  return (SUCCESS);
}
