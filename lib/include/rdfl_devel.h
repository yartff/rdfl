#ifndef		__RDFL_DEVEL_H_
# define	__RDFL_DEVEL_H_

# ifdef		DEVEL
#  include	"rdfl.h"

#  define	RDFL_IS_ASCII_PRINTABLECHAR(c) ((c) >= ' ' && (c) <= '~')
#  define	RDFL_IS_ASCII_NODISPLAYCHAR(c) (((c) == '\n') || ((c) == '\t'))

void		rdflDevel_printbufferstate(t_rdfl *obj);
void		*rdflDevel_get_func(e_rdflsettings);
const char	*rdflDevel_handler_typedef_declare(void *ptr);
const char	*rdflDevel_handler_func_declare(void *ptr);

# endif

#endif		/* !__RDFL_DEVEL_H_ */
