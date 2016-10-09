#ifndef			__RDFL_DEVEL_H_
# define		__RDFL_DEVEL_H_

// # ifdef		DEVEL
void			rdfl_printbufferstate(t_rdfl *obj);
void			*get_func(e_rdflsettings);
const char		*handler_typedef_declare(void *ptr);
const char		*handler_func_declare(void *ptr);
// # endif

#endif			/* !__RDFL_DEVEL_H_ */
