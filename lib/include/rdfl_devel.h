#ifndef		__RDFL_DEVEL_H_
# define	__RDFL_DEVEL_H_

# ifdef		DEVEL
# include	"rdfl.h"
# include	"rdfl_buffer.h"

void		rdfl_printbufferstate(t_rdfl *obj);
void		*get_func(e_rdflsettings);
const char	*handler_typedef_declare(void *ptr);
const char	*handler_func_declare(void *ptr);
void		rdfl_b_print_buffers(t_rdfl_buffer *);

// Overload
e_rdflerrors	rdfl_ctx_popuntil(t_rdfl_buffer *, ctxid_t);
e_rdflerrors	rdfl_dropallcontexts(t_rdfl_buffer *);
e_rdflerrors	rdfl_clean(t_rdfl *obj);
e_rdflerrors	rdfl_buffer_clean(t_rdfl_buffer *);
# endif

#endif		/* !__RDFL_DEVEL_H_ */
